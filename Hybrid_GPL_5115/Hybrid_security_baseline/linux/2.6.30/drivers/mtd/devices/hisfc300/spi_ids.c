/******************************************************************************
*    Copyright (c) 2009-2010 by czy.
*    All rights reserved.
* ***
*    Create by Cai ZhiYong. 2010-12-18
*
******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <asm/setup.h>

#include "spi_ids.h"

#define SPI_DRV_VERSION       "1.22"
/*****************************************************************************/

#if 1
#  define DBG_MSG(_fmt, arg...)
#else
#  define DBG_MSG(_fmt, arg...)   \
	printk("%s(%d): " _fmt, __FILE__, __LINE__, ##arg);
#endif

#define DBG_BUG(fmt, args...) do{\
	printk("%s(%d): BUG !!! " fmt, __FILE__, __LINE__, ##args); \
	while(1); \
} while (0)

/*****************************************************************************/

#define SET_READ_STD(_dummy_, _size_, _clk_) \
	static struct spi_operation read_std_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_STD, SPI_CMD_READ, _dummy_, _size_, _clk_ }

#define READ_STD(_dummy_, _size_, _clk_) read_std_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_READ_FAST(_dummy_, _size_, _clk_) \
	static struct spi_operation read_fast_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_FAST, SPI_CMD_FAST_READ, _dummy_, _size_, _clk_ }

#define READ_FAST(_dummy_, _size_, _clk_) read_fast_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_READ_DUAL(_dummy_, _size_, _clk_) \
	static struct spi_operation read_dual_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_DUAL, SPI_CMD_READ_DUAL, _dummy_, _size_, _clk_ }

#define READ_DUAL(_dummy_, _size_, _clk_) read_dual_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_READ_QUAD(_dummy_, _size_, _clk_) \
	static struct spi_operation read_quad_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD, SPI_CMD_READ_QUAD, _dummy_, _size_, _clk_ }

#define READ_QUAD(_dummy_, _size_, _clk_) read_quad_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_READ_DUAL_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_operation read_dual_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_DUAL_ADDR, SPI_CMD_READ_DUAL_ADDR, _dummy_, _size_, _clk_ }

#define READ_DUAL_ADDR(_dummy_, _size_, _clk_) read_dual_addr_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_READ_QUAD_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_operation read_quad_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD_ADDR, SPI_CMD_READ_QUAD_ADDR, _dummy_, _size_, _clk_ }

#define READ_QUAD_ADDR(_dummy_, _size_, _clk_) read_quad_addr_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_WRITE_STD(_dummy_, _size_, _clk_) \
	static struct spi_operation write_std_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_STD, SPI_CMD_PP, _dummy_, _size_, _clk_ }

#define WRITE_STD(_dummy_, _size_, _clk_) write_std_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_WRITE_DUAL(_dummy_, _size_, _clk_) \
	static struct spi_operation write_dual_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_DUAL, SPI_CMD_WRITE_DUAL, _dummy_, _size_, _clk_ }

#define WRITE_DUAL(_dummy_, _size_, _clk_) write_dual_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_WRITE_QUAD(_dummy_, _size_, _clk_) \
	static struct spi_operation write_quad_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_QUAD, SPI_CMD_WRITE_QUAD, _dummy_, _size_, _clk_ }

#define WRITE_QUAD(_dummy_, _size_, _clk_) write_quad_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_WRITE_DUAL_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_operation write_dual_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_DUAL_ADDR, SPI_CMD_WRITE_DUAL_ADDR, _dummy_, _size_, _clk_ }

#define WRITE_DUAL_ADDR(_dummy_, _size_, _clk_) write_dual_addr_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_WRITE_QUAD_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_operation write_quad_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_QUAD_ADDR, SPI_CMD_WRITE_QUAD_ADDR, _dummy_, _size_, _clk_ }

#define WRITE_QUAD_ADDR(_dummy_, _size_, _clk_) write_quad_addr_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_ERASE_SECTOR(_dummy_, _size_, _clk_) \
	static struct spi_operation erase_sector_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_SECTOR, SPI_CMD_SE, _dummy_, _size_, _clk_ }

#define ERASE_SECTOR(_dummy_, _size_, _clk_) erase_sector_##_dummy_##_size_##_clk_
//-----------------------------------------------------------------------------

#define SET_ERASE_CHIP(_dummy_, _size_, _clk_) \
	static struct spi_operation erase_chip_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_CHIP, SPI_CMD_BE, _dummy_, _size_, _clk_ }

#define ERASE_CHIP(_dummy_, _size_, _clk_) erase_chip_##_dummy_##_size_##_clk_

/*****************************************************************************/

SET_READ_STD(0, INFINITE, 0);
SET_READ_STD(0, INFINITE, 20);
SET_READ_STD(0, INFINITE, 32);
SET_READ_STD(0, INFINITE, 33);
SET_READ_STD(0, INFINITE, 40);
SET_READ_STD(0, INFINITE, 50);
SET_READ_STD(0, INFINITE, 54);
SET_READ_STD(0, INFINITE, 66);

SET_READ_FAST(1, INFINITE, 50);
SET_READ_FAST(1, INFINITE, 64);
SET_READ_FAST(1, INFINITE, 66);
SET_READ_FAST(1, INFINITE, 75);
SET_READ_FAST(1, INFINITE, 80);
SET_READ_FAST(1, INFINITE, 86);
SET_READ_FAST(1, INFINITE, 104);
SET_READ_FAST(1, INFINITE, 108);

SET_READ_DUAL(1, INFINITE, 64);
SET_READ_DUAL(1, INFINITE, 75);
SET_READ_DUAL(1, INFINITE, 80);
SET_READ_DUAL(1, INFINITE, 104);
SET_READ_DUAL(1, INFINITE, 108);

SET_READ_QUAD(1, INFINITE, 64);
SET_READ_QUAD(1, INFINITE, 70);
SET_READ_QUAD(1, INFINITE, 80);
SET_READ_QUAD(1, INFINITE, 108);

SET_READ_DUAL_ADDR(1, INFINITE, 56);
SET_READ_DUAL_ADDR(1, INFINITE, 64);
SET_READ_DUAL_ADDR(2, INFINITE, 64);
SET_READ_DUAL_ADDR(0, INFINITE, 80);
SET_READ_DUAL_ADDR(1, INFINITE, 80);
SET_READ_DUAL_ADDR(2, INFINITE, 108);

SET_READ_QUAD_ADDR(5, INFINITE, 64);
SET_READ_QUAD_ADDR(2, INFINITE, 56);
SET_READ_QUAD_ADDR(2, INFINITE, 80);
SET_READ_QUAD_ADDR(5, INFINITE, 108);
//-----------------------------------------------------------------------------
SET_WRITE_STD(0, 256, 0);
SET_WRITE_STD(0, 256, 50);
SET_WRITE_STD(0, 256, 64);
SET_WRITE_STD(0, 256, 66);
SET_WRITE_STD(0, 256, 75);
SET_WRITE_STD(0, 256, 80);
SET_WRITE_STD(0, 256, 86);
SET_WRITE_STD(0, 256, 104);
SET_WRITE_STD(0, 256, 108);

SET_WRITE_DUAL(0, 256, 64);
SET_WRITE_DUAL(0, 256, 75);
SET_WRITE_DUAL(0, 256, 108);

SET_WRITE_QUAD(0, 256, 64);
SET_WRITE_QUAD(0, 256, 70);
SET_WRITE_QUAD(0, 256, 80);
SET_WRITE_QUAD(0, 256, 108);

SET_WRITE_DUAL_ADDR(0, 256, 64);
SET_WRITE_DUAL_ADDR(0, 256, 108);

SET_WRITE_QUAD_ADDR(0, 256, 64);
SET_WRITE_QUAD_ADDR(0, 256, 108);
//-----------------------------------------------------------------------------

SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 50);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 64);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 66);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 75);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 86);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104);
SET_ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 108);

/*****************************************************************************/

static struct spi_info spi_info_table[] =
{
	/* boot tag */
	{"null",{0,0,0},3,0,0,0,{0},{0},{0},},

	/* name        id                id_len chipsize(Bytes) erasesize */                        
	{"at25fs010",  {0x1f, 0x66, 0x01}, 3,  _128K,  _32K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"at25fs040",  {0x1f, 0x66, 0x04}, 3,  _512K,  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
										   
	{"at25df041a", {0x1f, 0x44, 0x01}, 3,  _512K,  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"at25df641",  {0x1f, 0x48, 0x00}, 3,  _8M,    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
																 
	{"at26f004",   {0x1f, 0x04, 0x00}, 3,  _512K,  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"at26df081a", {0x1f, 0x45, 0x01}, 3,  _1M,    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"at26df161a", {0x1f, 0x46, 0x01}, 3,  _2M,    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"at26df321",  {0x1f, 0x47, 0x01}, 3,  _4M,    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
																 
	/* Macronix */
	{"mx25l4005a",  {0xc2, 0x20, 0x13}, 3, _512K,  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}}, 
	
	{"MX25L8006E",  {0xc2, 0x20, 0x14}, 3, _1M,    _64K, 3,
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 86), &READ_DUAL(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 86), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 86), 0}},

	{"MX25L1606E",  {0xc2, 0x20, 0x15}, 3, _2M,    _64K, 3,
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 86), &READ_DUAL(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 86), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 86), 0}},    

	{"mx25l3205d",  {0xc2, 0x20, 0x16}, 3, _4M,    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	{"MX25L6406E",  {0xc2, 0x20, 0x17}, 3, _8M,    _64K, 3,
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 86), &READ_DUAL(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 86), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 86), 0}},

	{"MX25L128", {0xc2, 0x20, 0x18}, 3, _16M, _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 108), 0}, 
		{&WRITE_STD(0, 256, 108), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 108), 0}},
	/*
	 The follow chips have the same chipid, but command have some difference

	{"MX25L12836E", {0xc2, 0x20, 0x18}, 3, _16M,   _64K, 3,
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 108), 0},  
		{&WRITE_STD(0, 256, 108), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 108), 0}},
		
	{"MX25L12845E", {0xc2, 0x20, 0x18}, 3, _16M,   _64K, 3,
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 108), 0},  
		{&WRITE_STD(0, 256, 108), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 108), 0}},
	*/

	{"MX25L25635E", {0xc2, 0x20, 0x19}, 3, _32M, _64K, 4, 
		{&READ_STD(0, INFINITE, 40/*50*/), &READ_FAST(1, INFINITE, 64/*80*/), &READ_DUAL_ADDR(1, INFINITE, 56/*70*/), &READ_QUAD_ADDR(2, INFINITE, 56/*70*/), 0},  
		{&WRITE_STD(0, 256, 64/*80*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 64/*80*/), 0}},

	{"mx25l1655d",  {0xc2, 0x26, 0x15}, 3, _2M,    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"mx25l12855e", {0xc2, 0x26, 0x18}, 3, _16M,   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
																 
	{"s25sl004a", {0x01, 0x02, 0x12}, 3, (_64K * 8),   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"s25sl008a", {0x01, 0x02, 0x13}, 3, (_64K * 16),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"s25sl016a", {0x01, 0x02, 0x14}, 3, (_64K * 32),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	{"S25FL064P", {0x01, 0x02, 0x16, 0x4d}, 4, (_64K * 128), _64K, 3, 
		{&READ_STD(0, INFINITE, 40), &READ_FAST(1, INFINITE, 104), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 104), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},

	{"s25sl064a", {0x01, 0x02, 0x16}, 3, (_64K * 128), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	/* Spansion */
	{"S25FL032P", {0x01, 0x02, 0x15, 0x4d}, 4, (_64K * 64),  _64K, 3, 
		{&READ_STD(0, INFINITE, 40), &READ_FAST(1, INFINITE, 104), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), &READ_DUAL_ADDR(0, INFINITE, 80), &READ_QUAD_ADDR(2, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 104), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},

	{"S25FL032A", {0x01, 0x02, 0x15}, 3, (_64K * 64),  _64K, 3, 
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 50), 0},  
		{&WRITE_STD(0, 256, 50), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 50), 0}},

	{"S25FL204K", {0x01, 0x40, 0x13}, 3, (_64K * 8),   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	{"S25FL128P-0", {0x01, 0x20, 0x18, 0x03, 0x00}, 5, (_256K * 64),  _256K, 3, 
		{&READ_STD(0, INFINITE, 40), &READ_FAST(1, INFINITE, 104), 0},  
		{&WRITE_STD(0, 256, 104), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},

	{"S25FL128P-1", {0x01, 0x20, 0x18, 0x03, 0x01}, 5, (_64K * 256),  _64K, 3, 
		{&READ_STD(0, INFINITE, 40), &READ_FAST(1, INFINITE, 104), 0},  
		{&WRITE_STD(0, 256, 104), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},
		
	{"S25FL129P0", {0x01, 0x20, 0x18, 0x4d, 0x00}, 5, (_256K * 64),  _256K, 3, 
		{&READ_STD(0, INFINITE, 40),&READ_FAST(1, INFINITE, 104), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), &READ_DUAL_ADDR(0, INFINITE, 80), &READ_QUAD_ADDR(2, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 104), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},

	{"S25FL129P1", {0x01, 0x20, 0x18, 0x4d, 0x01}, 5, (_64K * 256),  _64K,  3, 
		{&READ_STD(0, INFINITE, 40),&READ_FAST(1, INFINITE, 104), &READ_DUAL(1, INFINITE, 64), &READ_QUAD(1, INFINITE, 80), &READ_DUAL_ADDR(0, INFINITE, 80), &READ_QUAD_ADDR(2, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 104), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},

	/*
	The chip and chip W25Q16B have the same chipid, but clock frequency have some difference
	{"S25FL016K", {0xef, 0x40, 0x15}, 3, (_64K * 32),  _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 104), &READ_DUAL(1, INFINITE, 104), &READ_QUAD(1, INFINITE, 104), 0},  
		{&WRITE_STD(0, 256, 104), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},
	*/
														
	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", {0xbf, 0x25, 0x8d}, 3, (_64K * 8),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25vf080b", {0xbf, 0x25, 0x8e}, 3, (_64K * 16), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25vf016b", {0xbf, 0x25, 0x41}, 3, (_64K * 32), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25vf032b", {0xbf, 0x25, 0x4a}, 3, (_64K * 64), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25wf512",  {0xbf, 0x25, 0x01}, 3, (_64K * 1),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25wf010",  {0xbf, 0x25, 0x02}, 3, (_64K * 2),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25wf020",  {0xbf, 0x25, 0x03}, 3, (_64K * 4),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{ "sst25wf040",  {0xbf, 0x25, 0x04}, 3, (_64K * 8),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
																 
	/* ST Microelectronics -- newer production may have feature updates */
	{"m25p05",  {0x20, 0x20, 0x10}, 3, (_32K * 2), _32K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m25p10",  {0x20, 0x20, 0x11}, 3, (_32K * 4), _32K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	{"m25p20",  {0x20, 0x20, 0x12}, 3, (_64K * 4),   _64K,  3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m25p40",  {0x20, 0x20, 0x13}, 3, (_64K * 8),   _64K,  3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m25p80",  {0x20, 0x20, 0x14}, 3, (_64K * 16),  _64K,  3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m25p16",  {0x20, 0x20, 0x15}, 3, (_64K * 32),  _64K,  3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	
	{"M25P32",  {0x20, 0x20, 0x16, 0x10}, 4, _4M, _64K, 3,
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 75), 0},  
		{&WRITE_STD(0, 256, 75), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 75), 0}},
	
	{"m25p64",  {0x20, 0x20, 0x17}, 3, (_64K * 128), _64K,  3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	
	{"M25P128", {0x20, 0x20, 0x18}, 3, _16M, _256K, 3, 
		{&READ_STD(0, INFINITE, 20), &READ_FAST(1, INFINITE, 50), 0},  
		{&WRITE_STD(0, 256, 50), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 50), 0}},
																 
	{"m45pe10", {0x20, 0x40, 0x11}, 3, (_64K * 2),   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m45pe80", {0x20, 0x40, 0x14}, 3, (_64K * 16),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m45pe16", {0x20, 0x40, 0x15}, 3, (_64K * 32),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
																 
	{"m25pe80", {0x20, 0x80, 0x14}, 3, (_64K * 16), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"m25pe16", {0x20, 0x80, 0x15}, 3, (_64K * 32), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	{"N25Q032", {0x20, 0xba, 0x16}, 3, (_64K * 64), _64K, 3, 
		{&READ_STD(0, INFINITE, 32/*54*/), &READ_FAST(1, INFINITE, 64/*108*/), &READ_DUAL(1, INFINITE, 64/*108*/), &READ_QUAD(1, INFINITE, 64/*108*/), &READ_DUAL_ADDR(2, INFINITE, 64/*108*/), &READ_QUAD_ADDR(5, INFINITE, 64/*108*/), 0},  
		{&WRITE_STD(0, 256, 64/*108*/), &WRITE_DUAL(0, 256, 64/*108*/), &WRITE_QUAD(0, 256, 64/*108*/), &WRITE_DUAL_ADDR(0, 256, 64/*108*/), &WRITE_QUAD_ADDR(0, 256, 64/*108*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 64/*108*/), 0}},
															   
	{"N25Q128",   {0x20, 0xba, 0x18}, 3, (_64K * 256), _64K, 3, 
		{&READ_STD(0, INFINITE, 54), &READ_FAST(1, INFINITE, 108), &READ_DUAL(1, INFINITE, 108), &READ_QUAD(1, INFINITE, 108), &READ_DUAL_ADDR(2, INFINITE, 108), &READ_QUAD_ADDR(5, INFINITE, 108), 0},  
		{&WRITE_STD(0, 256, 108), &WRITE_DUAL(0, 256, 108), &WRITE_QUAD(0, 256, 108), &WRITE_DUAL_ADDR(0, 256, 108), &WRITE_QUAD_ADDR(0, 256, 108), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 108), 0}},

	{"M25PX16",  {0x20, 0x71, 0x15}, 3, (_64K * 32),  _64K, 3, 
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 75), &READ_DUAL(1, INFINITE, 75), 0},  
		{&WRITE_STD(0, 256, 75), &WRITE_DUAL(0, 256, 75), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 75), 0}},
  
	{"M25PX32", {0x20, 0x71, 0x16}, 3, (_64K * 64),  _64K, 3, 
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 75), &READ_DUAL(1, INFINITE, 75), 0}, 
		{&WRITE_STD(0, 256, 75), &WRITE_DUAL(0, 256, 75), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 75), 0}},

	{"m25px64",  {0x20, 0x71, 0x17}, 3, (_64K * 128), _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
																 
	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{"w25x10",  {0xef, 0x30, 0x11}, 3, (_64K * 2),    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"w25x20",  {0xef, 0x30, 0x12}, 3, (_64K * 4),    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"w25x40",  {0xef, 0x30, 0x13}, 3, (_64K * 8),    _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"w25x80",  {0xef, 0x30, 0x14}, 3, (_64K * 16),   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"w25x16",  {0xef, 0x30, 0x15}, 3, (_64K * 32),   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"w25x32",  {0xef, 0x30, 0x16}, 3, (_64K * 64),   _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},
	{"w25x64",  {0xef, 0x30, 0x17}, 3, (_64K * 128),  _64K, 3, {&READ_STD(0, INFINITE, 0), 0},  {&WRITE_STD(0, 256, 0), 0}, {&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 0), 0}},

	{"W25Q80BV",  {0xef, 0x40, 0x14}, 3, (_64K * 16),   _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 80), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80), 0}},

	{"W25Q16(B/C)V/S25FL016K",  {0xef, 0x40, 0x15}, 3, (_64K * 32),   _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 80), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80), 0}},
	/*
	 The follow chips have the same chipid, but command have some difference
	{"W25Q16BV",  {0xef, 0x40, 0x15}, 3, (_64K * 32),   _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 80), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80), 0}},

	{"W25Q16CV",  {0xef, 0x40, 0x15}, 3, (_64K * 32),   _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 80), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80), 0}},
	*/
	
	{"W25Q32BV",  {0xef, 0x40, 0x16}, 3, (_64K * 64),   _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80), &READ_DUAL(1, INFINITE, 80), &READ_QUAD(1, INFINITE, 80), 0},  
		{&WRITE_STD(0, 256, 80), &WRITE_QUAD(0, 256, 80), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80), 0}},
  
	{"W25Q128BV", {0xEF, 0x40, 0x18}, 3, _16M, _64K, 3, 
		{&READ_STD(0, INFINITE, 33), &READ_FAST(1, INFINITE, 104), &READ_DUAL(1, INFINITE, 104), &READ_QUAD(1, INFINITE, 70), 0},  
		{&WRITE_STD(0, 256, 104), &WRITE_QUAD(0, 256, 70), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 104), 0}},

	/* Eon -- fit clock frequency of RDSR instruction*/        
	{"EN25F80", {0x1c, 0x31, 0x14}, 3, (_64K * 16),  _64K, 3, 
		{&READ_STD(0, INFINITE, 66), &READ_FAST(1, INFINITE, 66/*100*/), 0},
		{&WRITE_STD(0, 256, 66/*100*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 66/*100*/), 0}},

	{"EN25F16", {0x1c, 0x31, 0x15}, 3, _2M,  _64K, 3, 
		{&READ_STD(0, INFINITE, 66), &READ_FAST(1, INFINITE, 66/*100*/), 0},
		{&WRITE_STD(0, 256, 66/*100*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 66/*100*/), 0}},

	{"EN25Q32B", {0x1c, 0x30, 0x16}, 3, (_64K * 64),  _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80/*104*/), &READ_DUAL(1, INFINITE, 80), &READ_DUAL_ADDR(1, INFINITE, 80),/*&READ_QUAD(3, INFINITE, 80), need enable and reset instructions*/ 0},
		{&WRITE_STD(0, 256, 80/*104*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80/*104*/), 0}},

	{"EN25Q64", {0x1c, 0x30, 0x17}, 3, (_64K * 128),  _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80/*104*/), &READ_DUAL(1, INFINITE, 80), &READ_DUAL_ADDR(1, INFINITE, 80),/*&READ_QUAD(3, INFINITE, 80), need enable and reset instructions*/ 0},
		{&WRITE_STD(0, 256, 80/*104*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80/*104*/), 0}},

	{"EN25Q128", {0x1c, 0x30, 0x18}, 3, (_64K * 256),  _64K, 3, 
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 80/*104*/), &READ_DUAL(1, INFINITE, 64/*80*/), &READ_DUAL_ADDR(1, INFINITE, 64/*80*/),/*&READ_QUAD(3, INFINITE, 80), need enable and reset instructions*/ 0},
		{&WRITE_STD(0, 256, 80/*104*/), 0}, 
		{&ERASE_SECTOR(0, SPI_IF_ERASE_SECTOR, 80/*104*/), 0}},

	{0,{0},0,0,0,0,{0},{0},{0},},
};
/*****************************************************************************/

static char *int_to_size(unsigned long long size)
{
	int ix;
	static char buffer[20]; 
	char *fmt[] = {"%u", "%uK", "%uM", "%uG", "%uT", "%uT"};

	for (ix = 0; (ix < 5) && !(size & 0x3FF) && size; ix++)
	{
		size = (size >> 10);
	}
	sprintf(buffer, fmt[ix], size);
	return buffer;
}
/*****************************************************************************/

struct spi_info *spi_serach_ids(unsigned char ids[8])
{
	struct spi_info *info;
	struct spi_info *fit_info = NULL;

	for (info = spi_info_table; info->name; info++)
	{
		if (memcmp(info->id, ids, info->id_len))
			continue;

		if (fit_info == NULL 
			|| fit_info->id_len < info->id_len)
		{
			fit_info = info;
		}
	}
	return fit_info;
}
/*****************************************************************************/

void spi_search_rw(struct spi_info *spiinfo, struct spi_operation *spiop_rw,
	unsigned int iftype, unsigned int max_dummy, int is_read)
{
	int ix = 0;
	struct spi_operation **spiop, **fitspiop;

	for (fitspiop = spiop = (is_read ? spiinfo->read : spiinfo->write);
		(*spiop) && ix < MAX_SPI_OP; spiop++, ix++)
	{
		DBG_MSG("dump[%d] %s iftype:0x%02X\n", ix,
			(is_read ? "read" : "write"),
			(*spiop)->iftype);

		if (((*spiop)->iftype & iftype)
			&& ((*spiop)->dummy <= max_dummy)
			&& (*fitspiop)->iftype < (*spiop)->iftype)
		{
			fitspiop = spiop;                
		}
	}
	memcpy(spiop_rw, (*fitspiop), sizeof(struct spi_operation));
}
/*****************************************************************************/

void spi_get_erase(struct spi_info *spiinfo, struct spi_operation *spiop_erase,
	unsigned int *erasesize)
{
	int ix;

	(*erasesize) = spiinfo->erasesize;
	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spiinfo->erase[ix] == NULL)
			break;

		memcpy(&spiop_erase[ix], spiinfo->erase[ix], 
			sizeof(struct spi_operation));

		switch (spiop_erase[ix].size)
		{
		case SPI_IF_ERASE_SECTOR:
			spiop_erase[ix].size = spiinfo->erasesize;
			break;
		case SPI_IF_ERASE_CHIP:
			spiop_erase[ix].size = spiinfo->chipsize;
			break;
		}
		DBG_MSG("erase[%d]->erasesize:%s, cmd:0x%02X\n",
			ix, int_to_size(spiop_erase[ix].size), spiop_erase[ix].cmd);

		if ((int)(spiop_erase[ix].size) < _2K)
		{
			DBG_BUG("erase block size mistaken: spi->erase[%d].size:%s\n",
				ix, int_to_size(spiop_erase[ix].size));
		}

		if (spiop_erase[ix].size < (*erasesize))
		{
			(*erasesize) = spiop_erase[ix].size;
		}
	}
}
/*****************************************************************************/

struct spi_tag
{
	char name[16];

	unsigned char  id[8];
	unsigned int   id_len;

	unsigned long  chipsize;
	unsigned int   erasesize;
	unsigned int   addrcycle;

	struct spi_operation read[MAX_SPI_OP];
	struct spi_operation write[MAX_SPI_OP];
	struct spi_operation erase[MAX_SPI_OP];
};
/*****************************************************************************/

static int __init parse_spi_id(const struct tag *tag)
{
	int ix;
	static struct spi_tag spitag[1];
	struct spi_info *spiinfo = spi_info_table; 

	if (tag->hdr.size < ((sizeof (struct tag_header) + sizeof(struct spi_tag)) >> 2))
	{
		printk("%s(%d):	tag->hdr.size(%d) too small.\n",
			__FUNCTION__, __LINE__, tag->hdr.size);
		return 0;
	}
	memset(spiinfo, 0, sizeof(struct spi_info));
	memcpy(spitag, &tag->u, sizeof(struct spi_tag));

	spiinfo->name = spitag->name;

	memcpy(spiinfo->id, spitag->id, 8);
	spiinfo->id_len = spitag->id_len;

	spiinfo->chipsize = spitag->chipsize;
	spiinfo->erasesize = spitag->erasesize;
	spiinfo->addrcycle = spitag->addrcycle;

	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spitag->read[ix].iftype)
			spiinfo->read[ix] = &spitag->read[ix];
	}
	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spitag->write[ix].iftype)
			spiinfo->write[ix] = &spitag->write[ix];
	}
	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spitag->erase[ix].iftype)
			spiinfo->erase[ix] = &spitag->erase[ix];
	}
	
	printk("SPI TAG: hdr.tag: 0x%08X, hdr.size: %d\n", 
		tag->hdr.tag, tag->hdr.size);
	printk("(%dByte): 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
		spitag->id_len, 
		spitag->id[0], spitag->id[1], spitag->id[2], spitag->id[3], 
		spitag->id[4], spitag->id[5], spitag->id[6], spitag->id[7]);
	printk("Block:%sB ",     int_to_size(spitag->erasesize));
	printk("Chip:%sB ",      int_to_size(spitag->chipsize));
	printk("AddrCycle:%d ",  spitag->addrcycle);
	printk("Name:(%s)",      spitag->name);
	printk("\n");
	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spitag->read[ix].iftype)
		{
			printk("R %d: ", ix + 1);
			printk("IF Type:0x%02X ", spitag->read[ix].iftype);
			printk("CMD:0x%02X ",     spitag->read[ix].cmd);
			printk("Dummy:%d ",       spitag->read[ix].dummy);
			if (spitag->read[ix].size == INFINITE)
				printk("Size:-1      ");
			else
				printk("Size:%6sB ", int_to_size(spitag->read[ix].size));
			printk("Clock:%dMHz ",    spitag->read[ix].clock);
			printk("\n");
		}
	}
	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spitag->write[ix].iftype)
		{
			printk("W %d: ", ix + 1);
			printk("IF Type:0x%02X ", spitag->write[ix].iftype);
			printk("CMD:0x%02X ",     spitag->write[ix].cmd);
			printk("Dummy:%d ",       spitag->write[ix].dummy);
			printk("Size:%6sB ",      int_to_size(spitag->write[ix].size));
			printk("Clock:%dMHz ",    spitag->write[ix].clock);
			printk("\n");
		}
	}
	for (ix = 0; ix < MAX_SPI_OP; ix++)
	{
		if (spitag->erase[ix].iftype)
		{
			printk("E %d: ", ix + 1);
			printk("IF Type:0x%02X ", spitag->erase[ix].iftype);
			printk("CMD:0x%02X ",     spitag->erase[ix].cmd);
			printk("Dummy:%d ",       spitag->erase[ix].dummy);
			printk("Size:0x%02X ",    spitag->erase[ix].size);
			printk("Clock:%dMHz ",    spitag->erase[ix].clock);
			printk("\n");
		}
	}
	
	return 0;
}

/* turn to ascii is "S_ID" */
__tagtable(0x535F4944, parse_spi_id);
/*****************************************************************************/

static int __init spi_ids_init(void)
{
	printk("Spi id table Version %s\n", SPI_DRV_VERSION);
	return 0;
}
/*****************************************************************************/

static void __exit spi_ids_exit(void)
{
}
/*****************************************************************************/

module_init(spi_ids_init);
module_exit(spi_ids_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cai ZhiYong, caizhiyong@huawei.com");
MODULE_DESCRIPTION("Spi id table");
