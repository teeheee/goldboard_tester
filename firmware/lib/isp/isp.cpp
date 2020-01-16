#include "isp.h"
#include "SPI.h"

#define RESET     SS
#define PTIME 30

uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  uint8_t n;
  SPI.transfer(a); 
  n=SPI.transfer(b);
  n=SPI.transfer(c);
  return SPI.transfer(d);
}

void start_pmode() 
{
  SPI.begin();
  digitalWrite(RESET, HIGH);
  pinMode(RESET, OUTPUT);
  digitalWrite(SCK, LOW);
  delay(20);
  digitalWrite(RESET, LOW);
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
}

void end_pmode() 
{
  SPI.end();
  digitalWrite(RESET, HIGH);
  pinMode(RESET, INPUT);
}


void flash(uint8_t hilo, int addr, uint8_t data) {
  spi_transaction(
    0x40+8*hilo, 
    addr>>8 & 0xFF, 
    addr & 0xFF,
    data
  );
}

void commit(int addr) {
  spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
  delay(PTIME);
}

int current_page(int addr) {
  return addr & 0xFFFFFFC0;
}

void write_flash_pages(uint16_t address, uint8_t* buff, int length) {
  int x = 0;
  int page = current_page(address);
  while (x < length) {
    if (page != current_page(address)) {
      commit(page);
      page = current_page(address);
    }
    flash(LOW, address, buff[x++]);
    flash(HIGH, address, buff[x++]);
    address++;
  }
  commit(page);
}

void isp_init()
{
  SPI.setDataMode(0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
}

uint32_t isp_read_signature() 
{
  delay(PTIME);
  uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  delay(PTIME);
  uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  delay(PTIME);
  uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  delay(PTIME);
  return ((uint32_t)high<<16) | ((uint32_t)middle<<8) | (uint32_t)low;
}

void program_to_flash(uint8_t* buff, int length)
{
  delay(PTIME);
  write_flash_pages(0x3f00, buff, length);
  delay(PTIME);
}

uint8_t check_low_fuse(uint8_t low_fuse)
{
  delay(PTIME);
  if(low_fuse == spi_transaction(0x50, 0x00, 0x00, 0x00))
  {  
    return 1;
  }
  else
  {
    return 0;
  }
}

uint8_t check_high_fuse(uint8_t high_fuse)
{
  delay(PTIME);
  if(high_fuse == spi_transaction(0x58, 0x08, 0x00, 0x00))
  {  
    return 1;
  }
  else
  {
    return 0;
  }
}


void program_fuses(uint8_t high_fuse, uint8_t low_fuse)
{
  for(int i = 0; i < 4; i++)
    check_high_fuse(high_fuse);
  for(int i = 0; i < 4; i++)
    check_low_fuse(low_fuse);
  delay(PTIME);
  spi_transaction(0xAC, 0xA8, 0x00, low_fuse);
  for(int i = 0; i < 4; i++)
    check_high_fuse(high_fuse);
  for(int i = 0; i < 4; i++)
    check_low_fuse(low_fuse);
  delay(PTIME);
  spi_transaction(0xAC, 0xA0, 0x00, high_fuse);
  for(int i = 0; i < 4; i++)
    check_high_fuse(high_fuse);
  for(int i = 0; i < 4; i++)
    check_low_fuse(low_fuse);
  delay(PTIME);
}
