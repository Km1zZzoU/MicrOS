#include <micros/sys/micros_socket.h>
#include <inet/ipv4.h>
#include <micros/sys/micros_keyboard.h>

#define BUF_LEN 1500

int main(int argc, char *argv[])
{
    // UDP
    char buffer[BUF_LEN] = {0};
    uint32_t sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in server_addr = {0};

    socklen_t server_addr_len = sizeof(struct sockaddr_in);

    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(40025);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr));

    while (1)
    {
        uint32_t bytes_received = recvfrom(sock,
                                           buffer,
                                           sizeof(buffer),
                                           0,
                                           (struct sockaddr *)&server_addr,
                                           &server_addr_len);
        if (bytes_received)
        {
            printf("%.*s  ", bytes_received, buffer);
            printf("Sent: %d\n", sendto(sock, buffer, bytes_received, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)));
        }

        if (micros_keyboard_is_key_pressed())
        {
            micros_keyboard_scan_ascii_pair pressed_key;
            micros_keyboard_get_pressed_key(&pressed_key);

            switch (pressed_key.scancode)
            {
            case key_esc:
                return;
            }
        }
    }

    // TCP
    // int s;
    // struct sockaddr_in myaddr;
    // myaddr.sin_family = AF_INET;
    // myaddr.sin_port = htons(3490);       // clients connect to this port
    // myaddr.sin_addr.s_addr = INADDR_ANY; // autoselect IP address

    // s = socket(AF_INET, SOCK_STREAM, 0);
    // bind(s, (struct sockaddr *)&myaddr, sizeof(myaddr));

    // if(!listen(s, 2)); // set s up to be a server (listening) socket
    //     return -1;
    

    // then have an accept() loop down here somewhere

    return 0;
}