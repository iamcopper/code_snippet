#include <stdint.h>
#include <stddef.h>

uint8_t calculate_checksum(uint8_t *buf, size_t len)
{
	uint8_t cks = 0;
	size_t i;

	for (i = 0; i < len; i++)
		cks += buf[i];

	return cks;
}
