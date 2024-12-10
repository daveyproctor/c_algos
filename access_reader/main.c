/*
Take-Home Assignment for Firmware Engineer @ Resilient Lifescience.

Please limit yourself to two hours on this assignment. Feel free to use any
resources available on the internet, as long as you cite them. This includes
documentation, sample code, research papers, ChatGPT, or whatever else you might
find useful.

Your task is to write the firmware for an access card reader that locks /
unlocks the door to a secure facility.

To enter the facility:
1. The user holds the key fob up to the access reader, which causes the key fob
to transmit an access code.
2. The access reader compares the code against its internal database.
3. If the code is valid, the access reader opens the door.

The list of codes changes over time. Codes are received wirelessly on a variable
schedule and each code comes with an expiration time. Codes are only valid
before the expiration time.

Assume that there are ~ 20k codes in the system at a given time.

The target device (the access reader) has the following hardware:
* microcontroller with:
  * enough flash for your binary
  * a minimal amount of RAM (e.g. 16 KBytes)
* external non-volatile flash to store the access codes: 1 MByte
  * See the flash.{h,c} library functions flash_write and flash_read.

For the purposes of this exercise, the code will be compiled and run on a
desktop computer using gcc. I have provided a simple Makefile to get you
started. To compile and run the project, use the following:

$ make
$ ./reader

Please write the firmware as if this were truly running on an embedded system,
like the one described.
*/

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

#include "string.h"
#include "flash.h"

// Number of bytes in the receive_access_code packet.
#define UPDATE_SIZE_BYTES 40

// Number of bytes in the access code.
#define ACCESS_CODE_BYTES 32

// Number of blocks to read at once from flash
#define READ_BLOCKS_SIZE 100

// Identifies the current door. Ignore requests with a different door_id.
#define MY_DOOR_ID 6

typedef uint8_t access_code_t[ACCESS_CODE_BYTES];

typedef struct __attribute__((__packed__)) {
  uint32_t expiration; // >0 if used
  access_code_t access_code; 
} storage_block_t;

typedef struct __attribute__((__packed__)) {
  uint16_t door_id;
  uint32_t expiration;
  uint16_t padding;
  access_code_t access_code;
} packet_t;

uint32_t hash(access_code_t access_code){
  uint32_t some_large_hash_number = access_code[0];
  return some_large_hash_number % (FLASH_MEMORY_SIZE / sizeof(storage_block_t));
}

// moves all blocks underneath this address up one to maintain continuity.
// it's possible block one is hashed to 100, and block 2 to 101. In this case
// it is incorrect to shift block 2 just because block 1 is expired.
// so you only shift if this hash(block.access_code) is less than the current index.
// nevertheless, you keep going until you find an empty block, since some blocks
// might still need shifting even if they're underneath an unshifted block.
// therefore, you should store the last free / shiftable-to location.
void expire_block(uint32_t storage_block_idx){
  // TODO
  return;
}

// Receive a wireless update with the access code.
//
// The update is 40 bytes and has the following format:
// [ door_id ][ expiration ][ padding ][ access_code ]
//
// * door_id is a 2-byte unsigned integer (little-endian) and identifies the
//   door. Ignore requests with door_id != MY_DOOR_ID
// * expiration is a four-byte unsigned integer (little-endian) and represents
//  the timestamp at which this access code expires, expressed in seconds since
//  the Unix epoch.
// * padding is two bytes, to be ignored.
// * access_code is the 32-byte access code
//
// The arguments to this function are:
// * current_time: the current time, expressed in seconds since the Unix epoch.
// * packet: bytes of the packet, always of size UPDATE_SIZE_BYTES.

// what the flash memory looks like:
// we use a "hash table with chaining" data structure, hashed by access_code.
// [storage_block_t] [storage_block_t] [empty] [empty] [...] [storage_block_t]
// ^ hashed by hash(access_code), and then placed in the next empty block.
// any expired blocks potentially need their children to be shifted leftward
// when they are removed so that continuity is maintained.
// see expire_block() above for further notes.
void receive_access_code(uint32_t current_time, uint8_t *packet) {
  // TODO: Implement this function.
  packet_t packet_parse;
  memcpy(&packet_parse, packet, sizeof(packet_t));
  uint32_t storage_block_idx = hash(packet_parse.access_code);

  if (packet_parse.door_id != MY_DOOR_ID || packet_parse.expiration < current_time){
    return;
  }

  storage_block_t new_block;
  new_block.expiration = packet_parse.expiration;
  memcpy(&new_block.access_code, &packet_parse.access_code, ACCESS_CODE_BYTES);

  storage_block_t storage_blocks [READ_BLOCKS_SIZE];
  flash_read(storage_block_idx * sizeof(storage_block_t), (uint8_t *) &storage_blocks, 
  READ_BLOCKS_SIZE * sizeof(storage_block_t));
  for (int i = 0; i < READ_BLOCKS_SIZE; i++){
    storage_block_t this_block; 
    memcpy(&this_block, &storage_blocks[i], sizeof(storage_block_t));
    if (this_block.expiration == 0){
      // the new block can go here.
      flash_write((storage_block_idx+i) * sizeof(storage_block_t), 
      (uint8_t *) &new_block, sizeof(storage_block_t));
      return;
    }
    if (memcmp(&this_block.access_code, &new_block.access_code, ACCESS_CODE_BYTES) == 0){
      // this access code is already there; maybe needs updating expiry
      if (this_block.expiration < new_block.expiration){
        flash_write((storage_block_idx+i) * sizeof(storage_block_t), 
        (uint8_t *) &new_block, sizeof(storage_block_t));
      }
      return;
    } else if (current_time > this_block.expiration){
      // expired
      expire_block(storage_block_idx + i);
    }
  }
  printf("Failed to find space for new access code\n");
  return;
}

// Returns true if this access code is valid. The door will unlock.
//
// The arguments to this function are:
// * current_time: the current time, expressed in seconds since the Unix epoch.
// * code: the access code to check, always of size ACCESS_CODE_BYTES.
bool unlock_door(uint32_t current_time, uint8_t *code) {
  // TODO: Implement this function.
  access_code_t access_code;
  memcpy(&access_code, code, ACCESS_CODE_BYTES);
  uint32_t storage_block_idx = hash(access_code);
  storage_block_t storage_blocks [READ_BLOCKS_SIZE];
  flash_read(storage_block_idx * sizeof(storage_block_t), (uint8_t *) &storage_blocks, 
  READ_BLOCKS_SIZE * sizeof(storage_block_t));
  int i = 0;
  while(i < READ_BLOCKS_SIZE){
    storage_block_t this_block; 
    memcpy(&this_block, &storage_blocks[i], sizeof(storage_block_t));
    if (this_block.expiration == 0){
      // not found
      return false;
    }
    if (memcmp(this_block.access_code, access_code, ACCESS_CODE_BYTES) == 0){
      return current_time < this_block.expiration;
    }
    if (current_time > this_block.expiration){
      // expired
      expire_block(storage_block_idx + i);
    }
    i++;
  }
  return false;
}

int main(void) {
  access_code_t access_code = {0};
  access_code[0] = 100;

  packet_t packet;
  memset(&packet.access_code, 0, ACCESS_CODE_BYTES);
  packet.access_code[0] = 101;
  packet.door_id = MY_DOOR_ID;
  packet.expiration = 1000;
  packet.padding = 0;

  receive_access_code(100, (uint8_t *) &packet);
  printf("unlock_door valid: %d\n", unlock_door(10, (uint8_t *) &packet.access_code));
  // expired
  printf("unlock_door expired: %d\n", unlock_door(10000, (uint8_t *) &packet.access_code));
  // not present
  printf("unlock_door not present: %d\n", unlock_door(100, (uint8_t *) &access_code));

}


/*
memory of the disk

there are 20k access codes
you hash the access code modulo the size of the flash.
you put it in the next available space.
you evict anything that is expired.
this requires shifting everything upwards.

receive_access_code:
parse
hash the access code
go to that memory address
while this block is not empty:
  if access_code is me:
    update expiry
  else if is expired:
    expire_block(address)
store access code with expiry in the next available spot.

On unlock_door:
hash the access code
go to that memory address
while this block is not empty:
  if block is not expired
    if access_code is me:
      return true
  else
    expire_block(address)
return false
*/


