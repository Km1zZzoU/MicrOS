#include "ide_pci.h"

//! Current states of all hard drives.
extern harddisk_states harddisk_current_states;
//! Current configuration of harddisk
extern harddisk_configuration harddisk_current_configuration;

static channel_regs channels[2]; //Temporary. Array of channels on a single controller
static ide_device devices[4]; //Temporary. Array of all devices on a single controller

static prd_entry prd_table[64] __attribute__((aligned(0x10000)));

uint32_t bar4_address;
uint32_t bar4_command;
uint32_t bar4_control;
uint32_t bar4_prdt;

static bool waitForInterrupt;

int8_t ide_pci_check_presence(HARDDISK_ATA_MASTER_SLAVE type, HARDDISK_ATA_BUS_TYPE bus, harddisk_states *states)
{
    uint16_t io_port = 0;
    uint16_t control_port = 0;
    harddisk_io_drive_head_register message_to_drive = {.value = 0};

    // Set port of drive
    //Modify to use values from controller!
    if (bus == HARDDISK_ATA_PRIMARY_BUS) {io_port = channels[ATA_PRIMARY].base; control_port = channels[ATA_PRIMARY].control;}
    else if(bus == HARDDISK_ATA_SECONDARY_BUS) {io_port = channels[ATA_SECONDARY].base; control_port = channels[ATA_SECONDARY].control;}
    else return -2;

    // Set drive
    switch (type)
    {
    case HARDDISK_ATA_MASTER:
        // For master set it to 0xA0. We set 2 bits that should be always 1.
        message_to_drive.fields.always_set_field_1 = 1;
        message_to_drive.fields.always_set_field_2 = 1;
        break;
    case HARDDISK_ATA_SLAVE:
        // For slave set it to 0xB0. We set 2 bits that should be always 1 and drive number to 1.
        message_to_drive.fields.always_set_field_1 = 1;
        message_to_drive.fields.always_set_field_2 = 1;
        message_to_drive.fields.drive_number = 1;
        break;
    default:
        return -2;
    }

    harddisk_identify_device_data *data;
    if(type == HARDDISK_ATA_MASTER && bus == HARDDISK_ATA_PRIMARY_BUS) data = &states->primary_master_data;
    else if(type == HARDDISK_ATA_SLAVE && bus == HARDDISK_ATA_PRIMARY_BUS) data = &states->primary_slave_data;
    else if(type == HARDDISK_ATA_MASTER && bus == HARDDISK_ATA_SECONDARY_BUS) data = &states->secondary_master_data;
    else if(type == HARDDISK_ATA_SLAVE && bus == HARDDISK_ATA_SECONDARY_BUS) data = &states->secondary_slave_data;


    // Do soft reset
    int8_t result = __harddisk_soft_reset_port(control_port + HARDDISK_CONTROL_DEVICE_CONTROL_REGISTER_OFFSET);
    if(result == -1)
    {
        // Harddisk error
        return HARDDISK_ERROR;
    }

    // Send message to drive
    io_out_byte(io_port + HARDDISK_IO_DRIVE_HEAD_REGISTER_OFFSET, message_to_drive.value);


    // Make 400ns delay
    __harddisk_400ns_delay(control_port);


    // Poll the Status port until bit 7 (BSY, value = 0x80) clears.
    result = __harddisk_bsy_poll(control_port + HARDDISK_CONTROL_ALTERNATE_STATUS_REGISTER_OFFSET);
    if(result == -1)
    {
        // Harddisk error
        return HARDDISK_ERROR;
    }

    uint8_t cylinder_low = io_in_byte(io_port +  HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET);
    uint8_t cylinder_high = io_in_byte(io_port +  HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET);


    /* differentiate ATA, ATAPI, SATA and SATAPI */
    if (cylinder_low==0x3c && cylinder_high==0xc3)
    {
        return HARDDISK_SATA_PRESENT;
    }
    if (cylinder_low==0x69 && cylinder_high==0x96)
    {
        return HARDDISK_SATAPI_PRESENT;
    }
	if (cylinder_low==0x14 && cylinder_high==0xEB)
    {
        //__harddisk_get_identify_data(type, bus, data, HARDDISK_IDENTIFY_PACKET_DEVICE);
        return HARDDISK_ATAPI_PRESENT;
    }
	if (cylinder_low==0 && cylinder_high == 0)
    {
        ide__harddisk_get_identify_data(type, bus, data, 0xEC);
        return HARDDISK_ATA_PRESENT;
    }
	
	return HARDDISK_NOT_PRESENT;
}

bool ide_pci_init()
{
    
    static cntrlCount = 0;
    waitForInterrupt = false;
    for(int i = 0; i < pci_get_number_of_devices(); i++)
    {
        pci_device* dev = pci_get_device(i);
		char buff[64];
    
        if(dev->class_code == 0x1 && dev->subclass == 0x1)
        {
            logger_log_warning("NEW DEVICE");
            logger_log_warning(itoa(dev->device_id, buff, 16));
            //We kinda really need this thing
            pci_in_data data = pci_get_device_data(i);
            data.register_num = (0x04 & 0xFC) >> 2;
            data.enable = 1;
            uint32_t regVal = pci_get_register(&data);
            regVal |= 0x4;
            regVal &= 0xFDFF;
            pci_write_register(&data, regVal);

            data = pci_get_device_data(i);
            data.register_num = (0x04 & 0xFC) >> 2;
            data.enable = 1;
            regVal = pci_get_register(&data);

            // logger_log_info("IDE Controller found!");
            // logger_log_info(itoa(dev->vendor_id, buff, 16));
            // logger_log_info(itoa(dev->header_type, buff, 16));
            // logger_log_info(itoa(dev->class_code, buff, 16));
            // logger_log_info(itoa(dev->subclass, buff, 16));
            // logger_log_info(itoa(dev->prog_if, buff, 16));

            uint8_t status = dev->prog_if;
            if(status & 0x1)
                logger_log_info("Channel primary in native-PCI mode");
            else
                logger_log_info("Channel primary in compability mode");
            // if(status & 0x2)
            //     logger_log_info("Channel primary can have it's operation mode changed");
            // else
            //     logger_log_info("Channel primary has fixed operation mode");
            // if(status & 0x4)
            //     logger_log_info("Channel secondary in native-PCI mode");
            // else
            //     logger_log_info("Channel secondary in compability mode");
            // if(status & 0x8)
            //     logger_log_info("Channel secondary can have it's operation mode changed");
            // else
            //     logger_log_info("Channel secondary has fixed operation mode");
        
			//Read address from base register, or use default values
            //OSDEV special happened here.
            // According to PCI IDE specification device reports IO ports in BARs only if it is in native PCI mode.
            // That means we are supposed to completely ignore these adresses if we're in compat mode.
            // Current code is reading BARs properly, but since bit 0 of each BAR is set to 1 our effective port address becomes 0.
            // And this is causing detection code and later reading code to get fucked up.
            // Conclusion: ALWAYS verify twice information comming from wiki.osdev.org (ಥ _ ಥ)
            // Also comment from me (Minus): I think I might need to start working on articles on osdev...
            if(status & 0x1)
            {
                channels[ATA_PRIMARY].base = (dev->base_addres_0 & 0xFFFFFFFC) + 0x1F0 * (!dev->base_addres_0);
                channels[ATA_PRIMARY].control = (dev->base_addres_1 & 0xFFFFFFFC) + 0x3F6 * (!dev->base_addres_1);
                channels[ATA_SECONDARY].base = (dev->base_addres_2 & 0xFFFFFFFC) + 0x170 * (!dev->base_addres_2);
                channels[ATA_SECONDARY].control = (dev->base_addres_3 & 0xFFFFFFFC) + 0x376 * (!dev->base_addres_3);
            }
            else
            {
                channels[ATA_PRIMARY].base = 0x1F0;
                channels[ATA_PRIMARY].control = 0x3F6;
                channels[ATA_SECONDARY].base = 0x170;
                channels[ATA_SECONDARY].control = 0x376; 
            }
            logger_log_error(itoa(dev->base_addres_0, buff, 16));
            logger_log_error(itoa(dev->base_addres_1, buff, 16));
            logger_log_error(itoa(dev->base_addres_2, buff, 16));
            logger_log_error(itoa(dev->base_addres_3, buff, 16));

            logger_log_error(itoa(channels[ATA_PRIMARY].base, buff, 16));
            logger_log_error(itoa(channels[ATA_PRIMARY].control, buff, 16));
            logger_log_error(itoa(channels[ATA_SECONDARY].base, buff, 16));
            logger_log_error(itoa(channels[ATA_SECONDARY].control, buff, 16));

            logger_log_warning("Getting devices ready");

            //Check presence of devices on controller (each controller can at most have 2 channels)
            //Also current approach assumes we have only one controller on board (THIS IS BAD!!! (⊙x⊙;)
            harddisk_current_states.primary_master = (HARDDISK_STATE) ide_pci_check_presence(HARDDISK_ATA_MASTER, HARDDISK_ATA_PRIMARY_BUS, &harddisk_current_states);
            harddisk_current_states.primary_slave = (HARDDISK_STATE) ide_pci_check_presence(HARDDISK_ATA_SLAVE, HARDDISK_ATA_PRIMARY_BUS, &harddisk_current_states);
            harddisk_current_states.secondary_master = (HARDDISK_STATE) ide_pci_check_presence(HARDDISK_ATA_MASTER, HARDDISK_ATA_SECONDARY_BUS, &harddisk_current_states);
            harddisk_current_states.secondary_slave = (HARDDISK_STATE) ide_pci_check_presence(HARDDISK_ATA_SLAVE, HARDDISK_ATA_SECONDARY_BUS, &harddisk_current_states);

            logger_log_warning("Devices found and identified");

            //Soft reset both channels.

            {
			//What the actual fuck is this? (╯°□°）╯︵ ┻━┻
            //channels[ATA_PRIMARY].master = (dev->base_addres_4 & 0xFFFFFFFC);
			//channels[ATA_SECONDARY].master = (dev->base_addres_4 & 0xFFFFFFFC) + 8;

			// ide_write(&channels[ATA_PRIMARY]  , ATA_REG_CONTROL, 2);
			// ide_write(&channels[ATA_SECONDARY], ATA_REG_CONTROL, 2);
			// for (int j = 0; j < 2; j++)
            // {
			// 	for (int k = 0; k < 2; k++)
            //     {
            //         //do resets for each port
            //         int8_t resetRes = __harddisk_soft_reset_port(channels[j].control + HARDDISK_CONTROL_DEVICE_CONTROL_REGISTER_OFFSET);
            //         if(resetRes == -1)
            //         {
            //             continue;
            //         }


			// 		bool err = false;
			// 		uint8_t buffer[512];
			// 		ide_write(&channels[j], ATA_REG_HDDEVSEL, 0xA0 | k << 4);
			// 		sleep(10);
			// 		ide_write(&channels[j], ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
			// 		sleep(10);

			// 		uint8_t tmp =ide_read(&channels[j], ATA_REG_STATUS); 
			// 		logger_log_info(itoa(tmp, buff, 16)); //first 160000?
			// 		if( tmp == 0) 
			// 			logger_log_info("NO DRIVE!");
			// 		else
			// 		{
			// 			uint8_t pStatus = 0;
            //             char buf[64] = "STATUS CHANGED: ";
            //             //logger_log_info("Something detected");
			// 			while(1)
            //             {
            //                 status = ide_read(&channels[j], ATA_REG_STATUS);
			// 				if(pStatus != status)
            //                 {
            //                     itoa(status, &buf[16], 16);
            //                     logger_log_warning(buf);
            //                 }
            //                 pStatus = status;

            //                 if ((status & ATA_SR_ERR)) {
			// 					logger_log_info("Error while identifying drive!");
			// 					err = true;
			// 					break;
			// 				} // If Err, Device is not ATA.
			// 				if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)){
			// 					logger_log_info("Ready to read!");

    		// 					harddisk_identify_device_data *data;

			// 					if(j == 0 && k == 0)
			// 					{
			// 						harddisk_current_states.primary_master = HARDDISK_ATA_PRESENT;
			// 						data = &harddisk_current_states.primary_master_data;
			// 					}
			// 					if(j == 0 && k == 1)
			// 					{
			// 						harddisk_current_states.primary_slave = HARDDISK_ATA_PRESENT;
			// 						data = &harddisk_current_states.primary_slave_data;
			// 					}
			// 					if(j == 1 && k == 0)
			// 					{
			// 						harddisk_current_states.secondary_master = HARDDISK_ATA_PRESENT;
			// 						data = &harddisk_current_states.secondary_master_data;
			// 					}
			// 					if(j == 1 && k == 1)
			// 					{
			// 						harddisk_current_states.secondary_slave = HARDDISK_ATA_PRESENT;
			// 						data = &harddisk_current_states.secondary_slave_data;
			// 					}
			// 					ide__harddisk_get_identify_data(k+1,j+1, data, ATA_CMD_IDENTIFY);

			// 					break; // Everything is right.
			// 				}
			// 			}

			// 			if(!err){
			// 				for(int l = 0; l < 512; l++){
			// 					buffer[l] = ide_read(&channels[j], ATA_REG_DATA);
			// 				}
			// 			}
			// 		}
			// 	}
			// }
            }

            //Remeber to disable SATA controller for testing purposes
            bar4_address = dev->base_addres_4 & 0xFFFFFFFC;

            bar4_command = bar4_address;
            bar4_control = bar4_address+2;
            bar4_prdt = bar4_address+4;

            logger_log_error(itoa(bar4_command, buff, 16));
            logger_log_error(itoa(bar4_control, buff, 16));
            logger_log_error(itoa(bar4_prdt, buff, 16));


            //uint16_t bar4port = *((uint16_t*)bar4_command);

            uint32_t phys = paging_virtual_to_physical_address((uint32_t)prd_table);
            logger_log_info(itoa(phys, buff, 16));

            io_out_long(bar4_prdt, paging_virtual_to_physical_address(prd_table));

            pic_enable_irq(14);
            idt_attach_interrupt_handler(14, ide_pci_irq_handle);

			
            //dev->base_addres_4 = 0x50000;
            //prd_table = 0x50000;
        }
    }
	return false;
}

void ide_write(channel_regs* channel, uint8_t reg, uint8_t data){
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->ni);
	if (reg < 0x08)
		io_out_byte(channel->base  + reg - 0x00, data);
	else if (reg < 0x0C)
		io_out_byte(channel->base  + reg - 0x06, data);
	else if (reg < 0x0E)
		io_out_byte(channel->control  + reg - 0x0A, data);
	else if (reg < 0x16)
		io_out_byte(channel->master + reg - 0x0E, data);
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channel->ni);
}

uint8_t ide_read(channel_regs* channel, uint8_t reg){
	unsigned char result;
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->ni);
	if (reg < 0x08)
		result = io_in_byte(channel->base + reg - 0x00);
	else if (reg < 0x0C)
		result = io_in_byte(channel->base  + reg - 0x06);
	else if (reg < 0x0E)
		result = io_in_byte(channel->control  + reg - 0x0A);
	else if (reg < 0x16)
		result = io_in_byte(channel->master + reg - 0x0E);
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channel->ni);
	return result;
}

int8_t ide__harddisk_get_identify_data(HARDDISK_ATA_MASTER_SLAVE type, HARDDISK_ATA_BUS_TYPE bus, harddisk_identify_device_data *data, uint8_t identify_command)
{
    uint16_t io_port = 0;
    uint16_t control_port = 0;
    harddisk_io_drive_head_register message_to_drive = {.value = 0};

    // Set port of drive
    if (bus == HARDDISK_ATA_PRIMARY_BUS)
    {
        io_port = channels[ATA_PRIMARY].base;
        control_port = channels[ATA_PRIMARY].control;
    }
    else if(bus == HARDDISK_ATA_SECONDARY_BUS)
    {
        io_port = channels[ATA_SECONDARY].base;
        control_port = channels[ATA_SECONDARY].control;
    }
    else 
    {
        return -2;
    }

    // Set drive
    switch (type)
    {
    case HARDDISK_ATA_MASTER:
        // For master set it to 0xA0. We set 2 bits that should be always 1.
        message_to_drive.fields.always_set_field_1 = 1;
        message_to_drive.fields.always_set_field_2 = 1;
        break;
    case HARDDISK_ATA_SLAVE:
        // For slave set it to 0xB0. We set 2 bits that should be always 1 and drive number to 1.
        message_to_drive.fields.always_set_field_1 = 1;
        message_to_drive.fields.always_set_field_2 = 1;
        message_to_drive.fields.drive_number = 1;
        break;
    default:
        return -2;
    }


    // Send message to drive
    io_out_byte(io_port + HARDDISK_IO_DRIVE_HEAD_REGISTER_OFFSET, message_to_drive.value);

    // Make 400ns delay
    __harddisk_400ns_delay(control_port);

    // Set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0
    io_out_word(io_port + HARDDISK_IO_SECTOR_COUNT_REGISTER_OFFSET, 0);
    io_out_word(io_port + HARDDISK_IO_SECTOR_NUMBER_REGISTER_OFFSET, 0);
    io_out_word(io_port + HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET, 0);
    io_out_word(io_port + HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET, 0);

    // Send the specific IDENTIFY command to the Command IO port.
    io_out_byte(io_port + HARDDISK_IO_COMMAND_REGISTER_OFFSET, identify_command);


    // Read the Status port again.
    harddisk_io_control_status_register result;
    result.value = io_in_byte(io_port + HARDDISK_IO_STATUS_REGISTER_OFFSET);

    // If the value read is 0, the drive does not exist.
    if(result.value == 0)
    {
        return 0;
    }  
    else
    {   // For any other value: poll the Status port until bit 7 (BSY, value = 0x80) clears.
        for(;;)
        {
            if(result.fields.busy == 0)
            {
                //Check LBA mid and high to check if device follows ATA specification
                uint8_t LBA_mid, LBA_hi;
                LBA_mid = io_in_byte(io_port + HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET);
                LBA_hi = io_in_byte(io_port + HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET);
                if(LBA_hi != 0 && LBA_mid != 0) return 1;
                //TODO
                //There should be a way to load the data from devices that do not follow the standard. Find this way

                // Otherwise, continue polling one of the Status ports until bit 3 (DRQ, value = 8) sets, or until bit 0 (ERR, value = 1) sets.
                for(;;)
                {
                    result.value = io_in_byte(io_port + HARDDISK_IO_STATUS_REGISTER_OFFSET);


                    if(result.fields.drive_ready == 1 || result.fields.overlapped_mode_service_request == 1)
                    {
                        for(int i = 0; i < 256; i++)
                        {
                            //  Read 256 16-bit values, and store them.
                            data->values[i] = io_in_word(io_port);
                        }
                        return 1;
                    }
                    else if(result.fields.error_occurred == 1 || result.fields.drive_fault_error == 1)
                    {
                        return -1;
                    }
                }
                
            }   
            result.value = io_in_byte(io_port + HARDDISK_IO_STATUS_REGISTER_OFFSET);
        }
    }
}

uint8_t* ide_read_data(int device_number, int sector, int count)
{
    char logBuffer[2048];
    ///logger_log_info(itoa(count*512, logBuffer, 16));
    //beware, this is for 512 byte sectors, we should add detection if disk is AF.

    // We need alignment to work, but it pagefaults... somehow
    uint8_t* buffer = heap_kernel_alloc(count*512, 0x10000);

    uint32_t virtOffset = 0;
    uint32_t addr = paging_virtual_to_physical_address(buffer);

    // logger_log_warning(itoa(addr, logBuffer, 16));

    // keyboard_scan_ascii_pair kb;
    // while(!keyboard_get_key_from_buffer(&kb));

    //TODO reconsider if all of this cannot be simplified since we can now properly align memory addresses?
    uint32_t sizeToPageEnd = 0x10000 - (addr & 0xFFFF);

    prd_entry e;

    uint32_t totalBytesToRead = count*512;

    uint32_t prdEntryCounter = 0;

    if(totalBytesToRead <= sizeToPageEnd)
    {
        e.address = addr;
        e.byte_count = totalBytesToRead;
        e.reserved = 0;
        e.eot = 1;
        prd_table[prdEntryCounter] = e;
        prdEntryCounter++;
    }
    else
    {
        //logger_log_info("NEED MOAR");
        e.address = addr;
        e.byte_count = sizeToPageEnd;
        e.reserved = 0;
        e.eot = 0;
        virtOffset += sizeToPageEnd;
        totalBytesToRead -= sizeToPageEnd;
        prd_table[prdEntryCounter] = e;
        prdEntryCounter++;
        while(true)
        {
            addr = paging_virtual_to_physical_address(buffer+virtOffset);
            sizeToPageEnd = 0x10000 - (addr & 0xFFFF);
            if(totalBytesToRead <= sizeToPageEnd)
            {
                e.address = addr;
                e.byte_count = totalBytesToRead;
                e.reserved = 0;
                e.eot = 1;
                prd_table[prdEntryCounter] = e;
                prdEntryCounter++;
                break;
            }
            else
            {
                e.address = addr;
                e.byte_count = sizeToPageEnd;
                e.reserved = 0;
                e.eot = 0;
                virtOffset += sizeToPageEnd;
                totalBytesToRead -= sizeToPageEnd;
                prd_table[prdEntryCounter] = e;
                prdEntryCounter++;
            }
        }
        //TODO: Add PRD fill for more than 4KB boundary reads!
    }

    uint8_t otherReg = io_in_byte(bar4_control);

    io_out_byte(bar4_command, 0);
    uint8_t controlReg = io_in_byte(bar4_control);
    io_out_byte(bar4_control, controlReg & ~(0x6));

    uint16_t io_port = channels[ATA_PRIMARY].base;

    harddisk_io_drive_head_register message_to_drive = {.value = 0};
    message_to_drive.fields.uses_lba = 1;

    //Change to Paweł's code with message (thing above)
    io_out_byte(io_port + HARDDISK_IO_DRIVE_HEAD_REGISTER_OFFSET, (0xe0 | 0 << 4 | (sector & 0x0f000000) >> 24));

    //send higher bytes
    io_out_byte(io_port + HARDDISK_IO_SECTOR_COUNT_REGISTER_OFFSET, (uint8_t)(count >> 8));
    io_out_byte(io_port + HARDDISK_IO_SECTOR_NUMBER_REGISTER_OFFSET, (uint8_t)(sector >> 24));
    //temporary, will be checked later, since LBA48 is 48 bits....
    io_out_byte(io_port + HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET, (uint8_t)(0));
    io_out_byte(io_port + HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET, (uint8_t)(0));
    //Send lower bytes
    io_out_byte(io_port + HARDDISK_IO_SECTOR_COUNT_REGISTER_OFFSET, (uint8_t)count);
    io_out_byte(io_port + HARDDISK_IO_SECTOR_NUMBER_REGISTER_OFFSET, (uint8_t)sector);
    io_out_byte(io_port + HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET, (uint8_t)(sector >> 8));
    io_out_byte(io_port + HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET, (uint8_t)(sector >> 16));


    // io_out_byte(io_port + HARDDISK_IO_SECTOR_NUMBER_REGISTER_OFFSET, (uint8_t)high_lba);
    // io_out_byte(io_port + HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET, (uint8_t)(high_lba >> 8));
    // io_out_byte(io_port + HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET, (uint8_t)(high_lba >> 16));
    // io_out_byte(io_port + HARDDISK_IO_SECTOR_COUNT_REGISTER_OFFSET, (uint16_t)count);
    // io_out_byte(io_port + HARDDISK_IO_SECTOR_NUMBER_REGISTER_OFFSET, (uint8_t)low_lba);
    // io_out_byte(io_port + HARDDISK_IO_CYLINDER_LOW_REGISTER_OFFSET, (uint8_t)(low_lba >> 8));
    // io_out_byte(io_port + HARDDISK_IO_CYLINDER_HIGH_REGISTER_OFFSET, (uint8_t)(low_lba >> 16));

    io_out_byte(io_port + HARDDISK_IO_COMMAND_REGISTER_OFFSET, 0x25);

    waitForInterrupt = true;

    io_out_byte(bar4_command, 0x8 | 0x1);


    logger_log_warning("WAITING FOR INTERRUPT!");
    // Wait for dma write to complete
    while (waitForInterrupt)
    {
        uint8_t bmrCtrl = io_in_byte(bar4_control);
        if(bmrCtrl & 0x2)
        {
            logger_log_warning("BUS MASTER ERROR!");
            keyboard_scan_ascii_pair kb;
            while(!keyboard_get_key_from_buffer(&kb));
        }
    }

    //logger_log_info(itoa(addr, logBuffer, 16));


    // for(int i = 0; i < 32; i++)
    // {
    //     for(int j = 0; j < 16; j++)
    //     {
    //         itoa(buffer[i*16 + j], &logBuffer[j*2], 16);
    //     }
    //     logBuffer[33] = 0;
    //     logger_log_info(logBuffer);
    //     keyboard_scan_ascii_pair kb;
    //     while(!keyboard_get_key_from_buffer(&kb));
    // }

    return buffer;
    //while(1);
}

bool ide_pci_irq_handle(interrupt_state* irq_state)
{
    logger_log_info("HDD IRQ RECEIVED!");
    uint8_t devCtrl = io_in_byte(channels[ATA_PRIMARY].control);
    uint8_t bmrCtrl = io_in_byte(bar4_control);
    if(bmrCtrl & (0x4 | 0x1) == 0)
    {
        logger_log_warning("SOME ERROR!");
        // keyboard_scan_ascii_pair kb;
        // while(!keyboard_get_key_from_buffer(&kb));
    }
    io_out_byte(bar4_command, 0x0);
    waitForInterrupt = false;
    return false;
}