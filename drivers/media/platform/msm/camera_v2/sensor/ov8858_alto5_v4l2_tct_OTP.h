/*         Modify History For This Module
* When           Who             What,Where,Why
* ------------------------------------------------------------------
* 14/03/07      Hu Jin       add OTP for s5k5e2_rio5_ctcc(Ewelly)
* 14/05/06      YU Qijiang   add OTP for s5k5e2_alto45_cmcc
* 14/05/26      Hu Jin       /4 for 1st format version.
* ------------------------------------------------------------------
*/

#ifndef OV8858_ALTO5_CMCC_V4L2_TCT_OTP
#define OV8858_ALTO5_CMCC_V4L2_TCT_OTP
#include "msm_camera_i2c.h"

//Debug log
#define OV8858_ALTO5_CMCC_OTP_DEBUG_ON 0


#ifdef CONFIG_TCT_8X16_IDOL3
#define OV8858_RG_Ratio_Typical  626//202//0.792760725 * 256;// sync with GS
#define OV8858_BG_Ratio_Typical  568//161//0.627300368 * 256;// sync with GS
#else
#define OV8858_RG_Ratio_Typical  327//202//0.792760725 * 256;// sync with GS
#define OV8858_BG_Ratio_Typical  308//161//0.627300368 * 256;// sync with GS
#endif

static struct msm_camera_i2c_client *ov8858_g_client;

struct ov8858_otp_struct {
int flag; // bit[7]: info, bit[6]:wb, bit[5]:vcm, bit[4]:lenc
int module_integrator_id;
int lens_id;
int vcm_id;
int driver_ic;
int production_year;
int production_month;
int production_day;
int rg_ratio;
int bg_ratio;
int lenc[240];
int checksum;
int VCM_start;
int VCM_end;
int VCM_dir;

};


static int32_t OV8858_write_i2c(uint32_t addr, uint16_t data)
{
    int32_t rc = -EFAULT;
	if (!ov8858_g_client)
        printk("FFFF OTP: OV8858 client null\n");
    else{
        rc = ov8858_g_client->i2c_func_tbl->i2c_write(ov8858_g_client,addr, data, MSM_CAMERA_I2C_BYTE_DATA);
		if(rc < 0)
            printk("FFFF OTP: write error\n");
    }
    return rc;
}
static int16_t OV8858_read_i2c(uint32_t addr)
{
	uint16_t *data;
	uint16_t temp=0;
    int32_t rc = -EFAULT;
	data=&temp;
    if (!ov8858_g_client)
        printk("FFFF OTP: OV8858 null\n");
    else{
        rc = ov8858_g_client->i2c_func_tbl->i2c_read(ov8858_g_client,addr,data, MSM_CAMERA_I2C_BYTE_DATA);
		if(rc < 0)
            printk("FFFF OTP: read error\n");
    }
    return temp;
}


// index: index of otp group. (1, 2)
// return:0, group index is empty
//1, group index has invalid data
//2, group index has valid data

int ov8858_check_otp_info(int index)
{
int flag;
//set 0x5002[3] to “0”
int temp1;
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x00 & 0x08) | (temp1 & (~0x08)));
OV8858_write_i2c(0x3d84, 0xC0);
//partial mode OTP write start address
OV8858_write_i2c(0x3d88, 0x70);
OV8858_write_i2c(0x3d89, 0x10);
// partial mode OTP write end address
OV8858_write_i2c(0x3d8A, 0x70);
OV8858_write_i2c(0x3d8B, 0x10);
// read otp into buffer
OV8858_write_i2c(0x3d81, 0x01);
mdelay(5);
flag = OV8858_read_i2c(0x7010);
//select group
if (index == 1)
{
	flag = (flag>>6) & 0x03;
}
else if (index == 2)
{
	flag = (flag>>4) & 0x03;
}
// clear otp buffer
OV8858_write_i2c(0x7010, 0x00);
//set 0x5002[3] to “1”
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x08 & 0x08) | (temp1 & (~0x08)));

if (flag == 0x00) {
	return 0;
}
else if (flag & 0x02) {
	return 1;
}
else {
	return 2;
}

}

// index: index of otp group. (1, 2)
// return:	0, group index is empty
//		1, group index has invalid data
//		2, group index has valid data
int ov8858_check_otp_wb(int index)
{
int flag;
//set 0x5002[3] to “0”
int temp1;
OV8858_write_i2c(0x0100, 0x01);
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x00 & 0x08) | (temp1 & (~0x08)));
OV8858_write_i2c(0x3d84, 0xC0);
//partial mode OTP write start address
OV8858_write_i2c(0x3d88, 0x70);
OV8858_write_i2c(0x3d89, 0x1F);
// partial mode OTP write end address
OV8858_write_i2c(0x3d8A, 0x70);
OV8858_write_i2c(0x3d8B, 0x1F);
// read otp into buffer
OV8858_write_i2c(0x3d81, 0x01);
mdelay(5);

//select group
flag = OV8858_read_i2c(0x701F);
printk("OTP: flag = %d \n",flag);
if (index == 1)
{
	flag = (flag>>6) & 0x03;
}
else if (index == 2)
{
	flag = (flag>>4) & 0x03;
}

// clear otp buffer
OV8858_write_i2c( 0x701F, 0x00);

//set 0x5002[3] to “1”
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x08 & 0x08) | (temp1 & (~0x08)));

if (flag == 0x00) {
	return 0;
}
else if (flag & 0x02) {
	return 1;
}
else {
	return 2;
}
}

// index: index of otp group. (1, 2)
// return:	0, group index is empty
//		1, group index has invalid data
//		2, group index has valid data
int ov8858_check_otp_lenc(int index)
{
int flag;
//set 0x5002[3] to “0”
int temp1;
OV8858_write_i2c(0x0100, 0x01);
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x00 & 0x08) | (temp1 & (~0x08)));
OV8858_write_i2c(0x3d84, 0xC0);
//partial mode OTP write start address
OV8858_write_i2c(0x3d88, 0x70);
OV8858_write_i2c(0x3d89, 0x2D);
// partial mode OTP write end address
OV8858_write_i2c(0x3d8A, 0x70);
OV8858_write_i2c(0x3d8B, 0x2D);
// read otp into buffer
OV8858_write_i2c(0x3d81, 0x01);
mdelay(5);

//select group
flag = OV8858_read_i2c(0x702D);
printk("OTP: flag = %d \n",flag);
if (index == 1)
{
	flag = (flag>>6) & 0x03;
}
else if (index == 2)
{
	flag = (flag>>4) & 0x03;
}

// clear otp buffer
OV8858_write_i2c( 0x702D, 0x00);

//set 0x5002[3] to “1”
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x08 & 0x08) | (temp1 & (~0x08)));

if (flag == 0x00) {
	return 0;
}
else if (flag & 0x02) {
	return 1;
}
else {
	return 2;
}
}


// index: index of otp group. (1, 2)
// otp_ptr: pointer of ov8858_otp_struct
// return: 0,
int ov8858_read_otp_info(int index, struct ov8858_otp_struct *otp_ptr)
{
int i;
int start_addr, end_addr;

//set 0x5002[3] to “0”
int temp1;
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x00 & 0x08) | (temp1 & (~0x08)));

if (index == 1) {
	start_addr = 0x7011;
	end_addr = 0x7017;
}
else if (index == 2) {
	start_addr = 0x7018;
	end_addr = 0x701E;
}


OV8858_write_i2c(0x3d84, 0xC0);
//partial mode OTP write start address
OV8858_write_i2c(0x3d88, (start_addr >> 8) & 0xff);
OV8858_write_i2c(0x3d89, start_addr & 0xff);
// partial mode OTP write end address
OV8858_write_i2c(0x3d8A, (end_addr >> 8) & 0xff);
OV8858_write_i2c(0x3d8B, end_addr & 0xff);

// read otp into buffer
OV8858_write_i2c(0x3d81, 0x01);

mdelay(5);
(*otp_ptr).module_integrator_id = OV8858_read_i2c(start_addr);
(*otp_ptr).lens_id = OV8858_read_i2c(start_addr + 1);
(*otp_ptr).vcm_id = OV8858_read_i2c(start_addr + 2);
(*otp_ptr).driver_ic = OV8858_read_i2c(start_addr + 3);
(*otp_ptr).production_year = OV8858_read_i2c(start_addr + 4);
(*otp_ptr).production_month = OV8858_read_i2c(start_addr + 5);
(*otp_ptr).production_day = OV8858_read_i2c(start_addr + 6);

// clear otp buffer
for (i=start_addr; i<=end_addr; i++) {
	OV8858_write_i2c(i, 0x00);
}

//set 0x5002[3] to “1”
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x08 & 0x08) | (temp1 & (~0x08)));
return 0;
}

// index: index of otp group. (1, 2)
// otp_ptr: pointer of ov8858_otp_struct
// return:0,
int ov8858_read_otp_wb(int index, struct ov8858_otp_struct *otp_ptr)
{
int i;
int temp;
int start_addr, end_addr;
//int rg_gld,bg_gld;
//set 0x5002[3] to “0”
int temp1,temp2,temp3;//,temp2,temp3,temp4,temp5,temp6;
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x00 & 0x08) | (temp1 & (~0x08)));

if (index == 1) {
	start_addr = 0x7020;
	end_addr = 0x7022;
}
else if (index == 2) {
	start_addr = 0x7023;
	end_addr = 0x7025;
}

OV8858_write_i2c(0x3d84, 0xC0);
//partial mode OTP write start address
OV8858_write_i2c(0x3d88, (start_addr >> 8) & 0xff);
OV8858_write_i2c(0x3d89, start_addr & 0xff);
// partial mode OTP write end address
OV8858_write_i2c(0x3d8A, (end_addr >> 8) & 0xff);
OV8858_write_i2c(0x3d8B, end_addr & 0xff);


// read otp into buffer
OV8858_write_i2c(0x3d81, 0x01);

mdelay(5);
temp2 = OV8858_read_i2c(start_addr + 0);
//printk("[0x7020] = %d  \n",temp2);
temp3 = OV8858_read_i2c(start_addr + 1);
//printk("[0x7021] = %d  \n",temp3);
temp = OV8858_read_i2c(start_addr + 2);
//printk("[0x7022] = %d  \n",temp);
(*otp_ptr).rg_ratio = (OV8858_read_i2c(start_addr)<<2) + ((temp>>6) & 0x03);
(*otp_ptr).bg_ratio = (OV8858_read_i2c(start_addr + 1)<<2) + ((temp>>4) & 0x03);

//temp = OV8858_read_i2c(start_addr + 7);
//rg_gld = (OV8858_read_i2c(start_addr+4)<<2) + ((temp>>6) & 0x03);
//bg_gld = (OV8858_read_i2c(start_addr + 5)<<2) + ((temp>>4) & 0x03);
//printk("rg_gld: %d   bg_gld: %d \n",rg_gld,bg_gld);


// clear otp buffer
for (i=start_addr; i<=end_addr; i++) {
	OV8858_write_i2c(i, 0x00);
}
//set 0x5002[3] to “1”
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x08 & 0x08) | (temp1 & (~0x08)));
return 0;
}

// R_gain, sensor red gain of AWB, 0x400 =1
// G_gain, sensor green gain of AWB, 0x400 =1
// B_gain, sensor blue gain of AWB, 0x400 =1
// return 0;
int ov8858_update_awb_gain(int R_gain, int G_gain, int B_gain)
{
if (R_gain>0x400) {
	OV8858_write_i2c(0x5032, R_gain>>8);
	OV8858_write_i2c(0x5033, R_gain & 0x00ff);
}
if (G_gain>0x400) {
	OV8858_write_i2c(0x5034, G_gain>>8);
	OV8858_write_i2c(0x5035, G_gain & 0x00ff);
}

if (B_gain>0x400) {
	OV8858_write_i2c(0x5036, B_gain>>8);
	OV8858_write_i2c(0x5037, B_gain & 0x00ff);
}
return 0;
}

// call this function after OV8858 initialization
// return value: 0 update success
//		 1, no OTP
int ov8858_update_otp_wb(struct msm_camera_i2c_client *i2c_client)
{
struct ov8858_otp_struct current_otp;
int i;
int otp_index;
int temp;
int rg,bg,R_gain,B_gain,G_gain;
int nR_G_gain, nB_G_gain, nG_G_gain;
int nBase_gain;

ov8858_g_client = i2c_client;
// R/G and B/G of current camera module is read out from sensor OTP
// check first OTP with valid data
for(i=1;i<=2;i++) {
	temp = ov8858_check_otp_wb(i);
	if (temp == 2) {
	otp_index = i;
	break;
	}
}

if (i>2) {
	// no valid wb OTP data
	return 1;
}
//printk("index = %d",otp_index);
ov8858_read_otp_wb(otp_index, &current_otp);

rg = current_otp.rg_ratio ;
bg = current_otp.bg_ratio;

//calculate G gain

//printk("OTP:RG_G/RG_C=[%d:%d],BG_G/BG_C=[%d:%d]\n", OV8858_RG_Ratio_Typical, rg, OV8858_BG_Ratio_Typical, bg);
nR_G_gain = (OV8858_RG_Ratio_Typical*1000) / rg;
nB_G_gain = (OV8858_BG_Ratio_Typical*1000) / bg;
nG_G_gain = 1000;

if (nR_G_gain < 1000 || nB_G_gain < 1000)
{
if (nR_G_gain < nB_G_gain)
	nBase_gain = nR_G_gain;
else
	nBase_gain = nB_G_gain;
}
else
{
	nBase_gain = nG_G_gain;
}

R_gain = 0x400 * nR_G_gain / (nBase_gain);
B_gain = 0x400 * nB_G_gain / (nBase_gain);
G_gain = 0x400 * nG_G_gain / (nBase_gain);

ov8858_update_awb_gain(R_gain, G_gain, B_gain);
return 0;
}
int ov8858_read_otp_lenc(int index, struct ov8858_otp_struct *otp_ptr)
{
int i;
int start_addr, end_addr;
int temp1, sum_lsc = 0,check_sum = 0,sum_result = 0;
if (index == 1) {
start_addr = 0x702e;
end_addr = 0x711e;
}
else if (index == 2) {
start_addr = 0x711f;
end_addr = 0x720f;
}

temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x00 & 0x08) | (temp1 & (~0x08)));

OV8858_write_i2c(0x3d84, 0xC0);
//partial mode OTP write start address
OV8858_write_i2c(0x3d88, (start_addr >> 8) & 0xff);
OV8858_write_i2c(0x3d89, start_addr & 0xff);
// partial mode OTP write end address
OV8858_write_i2c(0x3d8A, (end_addr >> 8) & 0xff);
OV8858_write_i2c(0x3d8B, end_addr & 0xff);
// read otp into buffer
OV8858_write_i2c(0x3d81, 0x01);
mdelay(10);
for(i=0; i<240; i++) {

	
(* otp_ptr).lenc[i]=OV8858_read_i2c(start_addr+i);
sum_lsc += (* otp_ptr).lenc[i];
#if OV8858_ALTO5_CMCC_OTP_DEBUG_ON
printk("read lenc[%d] = %x \n",i,(* otp_ptr).lenc[i]);
#endif
}
sum_result = sum_lsc%255 + 1;
check_sum = OV8858_read_i2c(end_addr);
//#if OV8858_ALTO5_CMCC_OTP_DEBUG_ON
//printk("sum_lsc = %d,  sum_result = %d,  check_sum =%d  \n",sum_lsc,sum_result,check_sum);
//#endif
// clear otp buffer
for (i=start_addr; i<=end_addr; i++) {
OV8858_write_i2c(i, 0x00);
}
//set 0x5002[3] to “1”
temp1 = OV8858_read_i2c(0x5002);
OV8858_write_i2c(0x5002, (0x08 & 0x08) | (temp1 & (~0x08)));
if ( sum_result == check_sum )
return 1;
else
return 0;
}

// otp_ptr: pointer of otp_struct
int ov8858_update_lenc(struct ov8858_otp_struct * otp_ptr)
{
int i, temp;
temp = OV8858_read_i2c(0x5000);
temp = 0x80 | temp;
OV8858_write_i2c(0x5000, temp);
for(i=0;i<240;i++) {
OV8858_write_i2c(0x5800 + i, (*otp_ptr).lenc[i]);
#if OV8858_ALTO5_CMCC_OTP_DEBUG_ON
printk("write lenc[%d] = %x \n",i,(* otp_ptr).lenc[i]);
#endif
}
return 0;
}

// call this function after OV8858 initialization
// return value: 0 update success
//
//1, no OTP

int ov8858_update_otp_lenc(struct msm_camera_i2c_client *i2c_client)
{
struct ov8858_otp_struct current_otp;
int i;
int otp_index;
int temp;

ov8858_g_client = i2c_client;

// check first lens correction OTP with valid data
for(i=1;i<=2;i++) {
temp = ov8858_check_otp_lenc(i);
if (temp == 2) {
otp_index = i;
break;
}
}
if (i>2) {
// no valid WB OTP data
return 1;
}
if(ov8858_read_otp_lenc(otp_index, &current_otp))
ov8858_update_lenc(&current_otp);
// success
return 0;
}


// call this function after OV8858 initialization
// return value: 0 update success
//		 1, no OTP
static int ov8858_idol3_wb_once, ov8858_idol3_lenc_once;
int ov8858_idol3_update_otp_wb(struct msm_camera_i2c_client *i2c_client)
{
	struct ov8858_otp_struct current_otp;
	static int R_gain,B_gain,G_gain;
	int i;
	int otp_index;
	int temp;
	int rg,bg;
	int nR_G_gain, nB_G_gain, nG_G_gain;
	int nBase_gain;

	ov8858_g_client = i2c_client;

	if (0 == ov8858_idol3_wb_once) {
		// R/G and B/G of current camera module is read out from sensor OTP
		// check first OTP with valid data
		for(i=1;i<=2;i++) {
			temp = ov8858_check_otp_wb(i);
			if (temp == 2) {
			otp_index = i;
			break;
			}
		}

		if (i>2) {
			// no valid wb OTP data
			return 1;
		}
		//printk("index = %d",otp_index);
		ov8858_read_otp_wb(otp_index, &current_otp);

		rg = current_otp.rg_ratio ;
		bg = current_otp.bg_ratio;

		//calculate G gain

		//printk("OTP:RG_G/RG_C=[%d:%d],BG_G/BG_C=[%d:%d]\n", OV8858_RG_Ratio_Typical, rg, OV8858_BG_Ratio_Typical, bg);
		nR_G_gain = (OV8858_RG_Ratio_Typical*1000) / rg;
		nB_G_gain = (OV8858_BG_Ratio_Typical*1000) / bg;
		nG_G_gain = 1000;

		if (nR_G_gain < 1000 || nB_G_gain < 1000)
		{
			if (nR_G_gain < nB_G_gain)
				nBase_gain = nR_G_gain;
			else
				nBase_gain = nB_G_gain;
		}
		else
		{
			nBase_gain = nG_G_gain;
		}

		R_gain = 0x400 * nR_G_gain / (nBase_gain);
		B_gain = 0x400 * nB_G_gain / (nBase_gain);
		G_gain = 0x400 * nG_G_gain / (nBase_gain);

		ov8858_idol3_wb_once = 1;
	}

	if (1 == ov8858_idol3_wb_once) {
		printk("ov8858 R_gain(%d), G_gain(%d), B_gain(%d) \n", R_gain, G_gain, B_gain);
		ov8858_update_awb_gain(R_gain, G_gain, B_gain);
	}

	return 0;
}

// call this function after OV8858 initialization
// return value: 0 update success
//
//1, no OTP

int ov8858_idol3_update_otp_lenc(struct msm_camera_i2c_client *i2c_client)
{
	static struct ov8858_otp_struct current_otp;
	int i, rc;
	int otp_index;
	int temp;

	ov8858_g_client = i2c_client;
	if (0 == ov8858_idol3_lenc_once) {
		// check first lens correction OTP with valid data
		for(i=1;i<=2;i++) {
			temp = ov8858_check_otp_lenc(i);
			if (temp == 2) {
				otp_index = i;
				break;
			}
		}
		if (i>2) {
		// no valid WB OTP data
		return 1;
		}
		rc = ov8858_read_otp_lenc(otp_index, &current_otp);
		if (0 == rc) {
			/* read OTP error */
			return 2;
		}
		ov8858_idol3_lenc_once = 1;
	}

	if (ov8858_idol3_lenc_once)
		ov8858_update_lenc(&current_otp);
	// success
	return 0;
}

#endif

