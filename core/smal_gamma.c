/**
 *      Copyright 2020 TSN°SHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-Nov-10 Active
 *              Last Alteration $Author: takeda $
 */

#include <stdint.h>
#include <math.h>

#include "smal_gamma.h"

int smal_gamma8_init(uint8_t *const tab, const double g, const uint8_t offset, const uint16_t full_scale)
{
    static uint16_t n;
    const int16_t count = full_scale + 1;

    if(full_scale > 0xff) {
	return -1;
    }
    for (n = 0; n < count; n++){
	tab[n] = (int)(((double)full_scale * pow((double)n / (double)full_scale, 1.0 / g)) + offset);
    }

    return 0;
}

