
/* The following types must have exact sizes: */

typedef unsigned char byte;        /* MUST be 8 bits */
typedef unsigned short word16;     /* MUST be 16 bits */

void CRC_Init(byte *table);        /* Initialise. Passed the address of */
                                   /* a 512-byte buffer used for workspace */
void CRC_Clear(void);              /* Reset the CRC */ 
void CRC_Update(byte a);           /* Add a byte to the CRC */
word16 CRC_Done(void);             /* Get the completed CRC */
byte *CRC_Table(void);             /* Return the workspace address */

