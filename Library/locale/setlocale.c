#include "../locale.h"

char *setlocale(int category, const char *locale)
{
    if (locale_state == NULL)
    {
        locale_state = calloc(1, sizeof(lconv));
    }

    // TODO: replace it with strcmp
    if (locale[0] == 'C')
    {
        __locale_set_neutral_locale(category);

        __locale_delete_char_field_value(locale_name);
        locale_name = __locale_set_char_field_value(locale);
    }

    return locale_name;
}