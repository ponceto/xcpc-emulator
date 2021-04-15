/*
 * cpu-z80a.c - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu-z80a-priv.h"

static const uint16_t DAATable[2048] = {
    0x0044, 0x0100, 0x0200, 0x0304, 0x0400, 0x0504, 0x0604, 0x0700,
    0x0808, 0x090c, 0x1010, 0x1114, 0x1214, 0x1310, 0x1414, 0x1510,
    0x1000, 0x1104, 0x1204, 0x1300, 0x1404, 0x1500, 0x1600, 0x1704,
    0x180c, 0x1908, 0x2030, 0x2134, 0x2234, 0x2330, 0x2434, 0x2530,
    0x2020, 0x2124, 0x2224, 0x2320, 0x2424, 0x2520, 0x2620, 0x2724,
    0x282c, 0x2928, 0x3034, 0x3130, 0x3230, 0x3334, 0x3430, 0x3534,
    0x3024, 0x3120, 0x3220, 0x3324, 0x3420, 0x3524, 0x3624, 0x3720,
    0x3828, 0x392c, 0x4010, 0x4114, 0x4214, 0x4310, 0x4414, 0x4510,
    0x4000, 0x4104, 0x4204, 0x4300, 0x4404, 0x4500, 0x4600, 0x4704,
    0x480c, 0x4908, 0x5014, 0x5110, 0x5210, 0x5314, 0x5410, 0x5514,
    0x5004, 0x5100, 0x5200, 0x5304, 0x5400, 0x5504, 0x5604, 0x5700,
    0x5808, 0x590c, 0x6034, 0x6130, 0x6230, 0x6334, 0x6430, 0x6534,
    0x6024, 0x6120, 0x6220, 0x6324, 0x6420, 0x6524, 0x6624, 0x6720,
    0x6828, 0x692c, 0x7030, 0x7134, 0x7234, 0x7330, 0x7434, 0x7530,
    0x7020, 0x7124, 0x7224, 0x7320, 0x7424, 0x7520, 0x7620, 0x7724,
    0x782c, 0x7928, 0x8090, 0x8194, 0x8294, 0x8390, 0x8494, 0x8590,
    0x8080, 0x8184, 0x8284, 0x8380, 0x8484, 0x8580, 0x8680, 0x8784,
    0x888c, 0x8988, 0x9094, 0x9190, 0x9290, 0x9394, 0x9490, 0x9594,
    0x9084, 0x9180, 0x9280, 0x9384, 0x9480, 0x9584, 0x9684, 0x9780,
    0x9888, 0x998c, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0045, 0x0101, 0x0201, 0x0305, 0x0401, 0x0505, 0x0605, 0x0701,
    0x0809, 0x090d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1001, 0x1105, 0x1205, 0x1301, 0x1405, 0x1501, 0x1601, 0x1705,
    0x180d, 0x1909, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2021, 0x2125, 0x2225, 0x2321, 0x2425, 0x2521, 0x2621, 0x2725,
    0x282d, 0x2929, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3025, 0x3121, 0x3221, 0x3325, 0x3421, 0x3525, 0x3625, 0x3721,
    0x3829, 0x392d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4001, 0x4105, 0x4205, 0x4301, 0x4405, 0x4501, 0x4601, 0x4705,
    0x480d, 0x4909, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5005, 0x5101, 0x5201, 0x5305, 0x5401, 0x5505, 0x5605, 0x5701,
    0x5809, 0x590d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x6025, 0x6121, 0x6221, 0x6325, 0x6421, 0x6525, 0x6625, 0x6721,
    0x6829, 0x692d, 0x7031, 0x7135, 0x7235, 0x7331, 0x7435, 0x7531,
    0x7021, 0x7125, 0x7225, 0x7321, 0x7425, 0x7521, 0x7621, 0x7725,
    0x782d, 0x7929, 0x8091, 0x8195, 0x8295, 0x8391, 0x8495, 0x8591,
    0x8081, 0x8185, 0x8285, 0x8381, 0x8485, 0x8581, 0x8681, 0x8785,
    0x888d, 0x8989, 0x9095, 0x9191, 0x9291, 0x9395, 0x9491, 0x9595,
    0x9085, 0x9181, 0x9281, 0x9385, 0x9481, 0x9585, 0x9685, 0x9781,
    0x9889, 0x998d, 0xa0b5, 0xa1b1, 0xa2b1, 0xa3b5, 0xa4b1, 0xa5b5,
    0xa0a5, 0xa1a1, 0xa2a1, 0xa3a5, 0xa4a1, 0xa5a5, 0xa6a5, 0xa7a1,
    0xa8a9, 0xa9ad, 0xb0b1, 0xb1b5, 0xb2b5, 0xb3b1, 0xb4b5, 0xb5b1,
    0xb0a1, 0xb1a5, 0xb2a5, 0xb3a1, 0xb4a5, 0xb5a1, 0xb6a1, 0xb7a5,
    0xb8ad, 0xb9a9, 0xc095, 0xc191, 0xc291, 0xc395, 0xc491, 0xc595,
    0xc085, 0xc181, 0xc281, 0xc385, 0xc481, 0xc585, 0xc685, 0xc781,
    0xc889, 0xc98d, 0xd091, 0xd195, 0xd295, 0xd391, 0xd495, 0xd591,
    0xd081, 0xd185, 0xd285, 0xd381, 0xd485, 0xd581, 0xd681, 0xd785,
    0xd88d, 0xd989, 0xe0b1, 0xe1b5, 0xe2b5, 0xe3b1, 0xe4b5, 0xe5b1,
    0xe0a1, 0xe1a5, 0xe2a5, 0xe3a1, 0xe4a5, 0xe5a1, 0xe6a1, 0xe7a5,
    0xe8ad, 0xe9a9, 0xf0b5, 0xf1b1, 0xf2b1, 0xf3b5, 0xf4b1, 0xf5b5,
    0xf0a5, 0xf1a1, 0xf2a1, 0xf3a5, 0xf4a1, 0xf5a5, 0xf6a5, 0xf7a1,
    0xf8a9, 0xf9ad, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0045, 0x0101, 0x0201, 0x0305, 0x0401, 0x0505, 0x0605, 0x0701,
    0x0809, 0x090d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1001, 0x1105, 0x1205, 0x1301, 0x1405, 0x1501, 0x1601, 0x1705,
    0x180d, 0x1909, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2021, 0x2125, 0x2225, 0x2321, 0x2425, 0x2521, 0x2621, 0x2725,
    0x282d, 0x2929, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3025, 0x3121, 0x3221, 0x3325, 0x3421, 0x3525, 0x3625, 0x3721,
    0x3829, 0x392d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4001, 0x4105, 0x4205, 0x4301, 0x4405, 0x4501, 0x4601, 0x4705,
    0x480d, 0x4909, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5005, 0x5101, 0x5201, 0x5305, 0x5401, 0x5505, 0x5605, 0x5701,
    0x5809, 0x590d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x0604, 0x0700, 0x0808, 0x090c, 0x0a0c, 0x0b08, 0x0c0c, 0x0d08,
    0x0e08, 0x0f0c, 0x1010, 0x1114, 0x1214, 0x1310, 0x1414, 0x1510,
    0x1600, 0x1704, 0x180c, 0x1908, 0x1a08, 0x1b0c, 0x1c08, 0x1d0c,
    0x1e0c, 0x1f08, 0x2030, 0x2134, 0x2234, 0x2330, 0x2434, 0x2530,
    0x2620, 0x2724, 0x282c, 0x2928, 0x2a28, 0x2b2c, 0x2c28, 0x2d2c,
    0x2e2c, 0x2f28, 0x3034, 0x3130, 0x3230, 0x3334, 0x3430, 0x3534,
    0x3624, 0x3720, 0x3828, 0x392c, 0x3a2c, 0x3b28, 0x3c2c, 0x3d28,
    0x3e28, 0x3f2c, 0x4010, 0x4114, 0x4214, 0x4310, 0x4414, 0x4510,
    0x4600, 0x4704, 0x480c, 0x4908, 0x4a08, 0x4b0c, 0x4c08, 0x4d0c,
    0x4e0c, 0x4f08, 0x5014, 0x5110, 0x5210, 0x5314, 0x5410, 0x5514,
    0x5604, 0x5700, 0x5808, 0x590c, 0x5a0c, 0x5b08, 0x5c0c, 0x5d08,
    0x5e08, 0x5f0c, 0x6034, 0x6130, 0x6230, 0x6334, 0x6430, 0x6534,
    0x6624, 0x6720, 0x6828, 0x692c, 0x6a2c, 0x6b28, 0x6c2c, 0x6d28,
    0x6e28, 0x6f2c, 0x7030, 0x7134, 0x7234, 0x7330, 0x7434, 0x7530,
    0x7620, 0x7724, 0x782c, 0x7928, 0x7a28, 0x7b2c, 0x7c28, 0x7d2c,
    0x7e2c, 0x7f28, 0x8090, 0x8194, 0x8294, 0x8390, 0x8494, 0x8590,
    0x8680, 0x8784, 0x888c, 0x8988, 0x8a88, 0x8b8c, 0x8c88, 0x8d8c,
    0x8e8c, 0x8f88, 0x9094, 0x9190, 0x9290, 0x9394, 0x9490, 0x9594,
    0x9684, 0x9780, 0x9888, 0x998c, 0x9a8c, 0x9b88, 0x9c8c, 0x9d88,
    0x9e88, 0x9f8c, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0605, 0x0701, 0x0809, 0x090d, 0x0a0d, 0x0b09, 0x0c0d, 0x0d09,
    0x0e09, 0x0f0d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1601, 0x1705, 0x180d, 0x1909, 0x1a09, 0x1b0d, 0x1c09, 0x1d0d,
    0x1e0d, 0x1f09, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2621, 0x2725, 0x282d, 0x2929, 0x2a29, 0x2b2d, 0x2c29, 0x2d2d,
    0x2e2d, 0x2f29, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3625, 0x3721, 0x3829, 0x392d, 0x3a2d, 0x3b29, 0x3c2d, 0x3d29,
    0x3e29, 0x3f2d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4601, 0x4705, 0x480d, 0x4909, 0x4a09, 0x4b0d, 0x4c09, 0x4d0d,
    0x4e0d, 0x4f09, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5605, 0x5701, 0x5809, 0x590d, 0x5a0d, 0x5b09, 0x5c0d, 0x5d09,
    0x5e09, 0x5f0d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x6625, 0x6721, 0x6829, 0x692d, 0x6a2d, 0x6b29, 0x6c2d, 0x6d29,
    0x6e29, 0x6f2d, 0x7031, 0x7135, 0x7235, 0x7331, 0x7435, 0x7531,
    0x7621, 0x7725, 0x782d, 0x7929, 0x7a29, 0x7b2d, 0x7c29, 0x7d2d,
    0x7e2d, 0x7f29, 0x8091, 0x8195, 0x8295, 0x8391, 0x8495, 0x8591,
    0x8681, 0x8785, 0x888d, 0x8989, 0x8a89, 0x8b8d, 0x8c89, 0x8d8d,
    0x8e8d, 0x8f89, 0x9095, 0x9191, 0x9291, 0x9395, 0x9491, 0x9595,
    0x9685, 0x9781, 0x9889, 0x998d, 0x9a8d, 0x9b89, 0x9c8d, 0x9d89,
    0x9e89, 0x9f8d, 0xa0b5, 0xa1b1, 0xa2b1, 0xa3b5, 0xa4b1, 0xa5b5,
    0xa6a5, 0xa7a1, 0xa8a9, 0xa9ad, 0xaaad, 0xaba9, 0xacad, 0xada9,
    0xaea9, 0xafad, 0xb0b1, 0xb1b5, 0xb2b5, 0xb3b1, 0xb4b5, 0xb5b1,
    0xb6a1, 0xb7a5, 0xb8ad, 0xb9a9, 0xbaa9, 0xbbad, 0xbca9, 0xbdad,
    0xbead, 0xbfa9, 0xc095, 0xc191, 0xc291, 0xc395, 0xc491, 0xc595,
    0xc685, 0xc781, 0xc889, 0xc98d, 0xca8d, 0xcb89, 0xcc8d, 0xcd89,
    0xce89, 0xcf8d, 0xd091, 0xd195, 0xd295, 0xd391, 0xd495, 0xd591,
    0xd681, 0xd785, 0xd88d, 0xd989, 0xda89, 0xdb8d, 0xdc89, 0xdd8d,
    0xde8d, 0xdf89, 0xe0b1, 0xe1b5, 0xe2b5, 0xe3b1, 0xe4b5, 0xe5b1,
    0xe6a1, 0xe7a5, 0xe8ad, 0xe9a9, 0xeaa9, 0xebad, 0xeca9, 0xedad,
    0xeead, 0xefa9, 0xf0b5, 0xf1b1, 0xf2b1, 0xf3b5, 0xf4b1, 0xf5b5,
    0xf6a5, 0xf7a1, 0xf8a9, 0xf9ad, 0xfaad, 0xfba9, 0xfcad, 0xfda9,
    0xfea9, 0xffad, 0x0055, 0x0111, 0x0211, 0x0315, 0x0411, 0x0515,
    0x0605, 0x0701, 0x0809, 0x090d, 0x0a0d, 0x0b09, 0x0c0d, 0x0d09,
    0x0e09, 0x0f0d, 0x1011, 0x1115, 0x1215, 0x1311, 0x1415, 0x1511,
    0x1601, 0x1705, 0x180d, 0x1909, 0x1a09, 0x1b0d, 0x1c09, 0x1d0d,
    0x1e0d, 0x1f09, 0x2031, 0x2135, 0x2235, 0x2331, 0x2435, 0x2531,
    0x2621, 0x2725, 0x282d, 0x2929, 0x2a29, 0x2b2d, 0x2c29, 0x2d2d,
    0x2e2d, 0x2f29, 0x3035, 0x3131, 0x3231, 0x3335, 0x3431, 0x3535,
    0x3625, 0x3721, 0x3829, 0x392d, 0x3a2d, 0x3b29, 0x3c2d, 0x3d29,
    0x3e29, 0x3f2d, 0x4011, 0x4115, 0x4215, 0x4311, 0x4415, 0x4511,
    0x4601, 0x4705, 0x480d, 0x4909, 0x4a09, 0x4b0d, 0x4c09, 0x4d0d,
    0x4e0d, 0x4f09, 0x5015, 0x5111, 0x5211, 0x5315, 0x5411, 0x5515,
    0x5605, 0x5701, 0x5809, 0x590d, 0x5a0d, 0x5b09, 0x5c0d, 0x5d09,
    0x5e09, 0x5f0d, 0x6035, 0x6131, 0x6231, 0x6335, 0x6431, 0x6535,
    0x0046, 0x0102, 0x0202, 0x0306, 0x0402, 0x0506, 0x0606, 0x0702,
    0x080a, 0x090e, 0x0402, 0x0506, 0x0606, 0x0702, 0x080a, 0x090e,
    0x1002, 0x1106, 0x1206, 0x1302, 0x1406, 0x1502, 0x1602, 0x1706,
    0x180e, 0x190a, 0x1406, 0x1502, 0x1602, 0x1706, 0x180e, 0x190a,
    0x2022, 0x2126, 0x2226, 0x2322, 0x2426, 0x2522, 0x2622, 0x2726,
    0x282e, 0x292a, 0x2426, 0x2522, 0x2622, 0x2726, 0x282e, 0x292a,
    0x3026, 0x3122, 0x3222, 0x3326, 0x3422, 0x3526, 0x3626, 0x3722,
    0x382a, 0x392e, 0x3422, 0x3526, 0x3626, 0x3722, 0x382a, 0x392e,
    0x4002, 0x4106, 0x4206, 0x4302, 0x4406, 0x4502, 0x4602, 0x4706,
    0x480e, 0x490a, 0x4406, 0x4502, 0x4602, 0x4706, 0x480e, 0x490a,
    0x5006, 0x5102, 0x5202, 0x5306, 0x5402, 0x5506, 0x5606, 0x5702,
    0x580a, 0x590e, 0x5402, 0x5506, 0x5606, 0x5702, 0x580a, 0x590e,
    0x6026, 0x6122, 0x6222, 0x6326, 0x6422, 0x6526, 0x6626, 0x6722,
    0x682a, 0x692e, 0x6422, 0x6526, 0x6626, 0x6722, 0x682a, 0x692e,
    0x7022, 0x7126, 0x7226, 0x7322, 0x7426, 0x7522, 0x7622, 0x7726,
    0x782e, 0x792a, 0x7426, 0x7522, 0x7622, 0x7726, 0x782e, 0x792a,
    0x8082, 0x8186, 0x8286, 0x8382, 0x8486, 0x8582, 0x8682, 0x8786,
    0x888e, 0x898a, 0x8486, 0x8582, 0x8682, 0x8786, 0x888e, 0x898a,
    0x9086, 0x9182, 0x9282, 0x9386, 0x9482, 0x9586, 0x9686, 0x9782,
    0x988a, 0x998e, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f,
    0x4003, 0x4107, 0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707,
    0x480f, 0x490b, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b,
    0x5007, 0x5103, 0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703,
    0x580b, 0x590f, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f,
    0x6027, 0x6123, 0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723,
    0x682b, 0x692f, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f,
    0x7023, 0x7127, 0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727,
    0x782f, 0x792b, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b,
    0x8083, 0x8187, 0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787,
    0x888f, 0x898b, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b,
    0x9087, 0x9183, 0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783,
    0x988b, 0x998f, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f,
    0xa0a7, 0xa1a3, 0xa2a3, 0xa3a7, 0xa4a3, 0xa5a7, 0xa6a7, 0xa7a3,
    0xa8ab, 0xa9af, 0xa4a3, 0xa5a7, 0xa6a7, 0xa7a3, 0xa8ab, 0xa9af,
    0xb0a3, 0xb1a7, 0xb2a7, 0xb3a3, 0xb4a7, 0xb5a3, 0xb6a3, 0xb7a7,
    0xb8af, 0xb9ab, 0xb4a7, 0xb5a3, 0xb6a3, 0xb7a7, 0xb8af, 0xb9ab,
    0xc087, 0xc183, 0xc283, 0xc387, 0xc483, 0xc587, 0xc687, 0xc783,
    0xc88b, 0xc98f, 0xc483, 0xc587, 0xc687, 0xc783, 0xc88b, 0xc98f,
    0xd083, 0xd187, 0xd287, 0xd383, 0xd487, 0xd583, 0xd683, 0xd787,
    0xd88f, 0xd98b, 0xd487, 0xd583, 0xd683, 0xd787, 0xd88f, 0xd98b,
    0xe0a3, 0xe1a7, 0xe2a7, 0xe3a3, 0xe4a7, 0xe5a3, 0xe6a3, 0xe7a7,
    0xe8af, 0xe9ab, 0xe4a7, 0xe5a3, 0xe6a3, 0xe7a7, 0xe8af, 0xe9ab,
    0xf0a7, 0xf1a3, 0xf2a3, 0xf3a7, 0xf4a3, 0xf5a7, 0xf6a7, 0xf7a3,
    0xf8ab, 0xf9af, 0xf4a3, 0xf5a7, 0xf6a7, 0xf7a3, 0xf8ab, 0xf9af,
    0x0047, 0x0103, 0x0203, 0x0307, 0x0403, 0x0507, 0x0607, 0x0703,
    0x080b, 0x090f, 0x0403, 0x0507, 0x0607, 0x0703, 0x080b, 0x090f,
    0x1003, 0x1107, 0x1207, 0x1303, 0x1407, 0x1503, 0x1603, 0x1707,
    0x180f, 0x190b, 0x1407, 0x1503, 0x1603, 0x1707, 0x180f, 0x190b,
    0x2023, 0x2127, 0x2227, 0x2323, 0x2427, 0x2523, 0x2623, 0x2727,
    0x282f, 0x292b, 0x2427, 0x2523, 0x2623, 0x2727, 0x282f, 0x292b,
    0x3027, 0x3123, 0x3223, 0x3327, 0x3423, 0x3527, 0x3627, 0x3723,
    0x382b, 0x392f, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f,
    0x4003, 0x4107, 0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707,
    0x480f, 0x490b, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b,
    0x5007, 0x5103, 0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703,
    0x580b, 0x590f, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f,
    0x6027, 0x6123, 0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723,
    0x682b, 0x692f, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f,
    0x7023, 0x7127, 0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727,
    0x782f, 0x792b, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b,
    0x8083, 0x8187, 0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787,
    0x888f, 0x898b, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b,
    0x9087, 0x9183, 0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783,
    0x988b, 0x998f, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f,
    0xfabe, 0xfbba, 0xfcbe, 0xfdba, 0xfeba, 0xffbe, 0x0046, 0x0102,
    0x0202, 0x0306, 0x0402, 0x0506, 0x0606, 0x0702, 0x080a, 0x090e,
    0x0a1e, 0x0b1a, 0x0c1e, 0x0d1a, 0x0e1a, 0x0f1e, 0x1002, 0x1106,
    0x1206, 0x1302, 0x1406, 0x1502, 0x1602, 0x1706, 0x180e, 0x190a,
    0x1a1a, 0x1b1e, 0x1c1a, 0x1d1e, 0x1e1e, 0x1f1a, 0x2022, 0x2126,
    0x2226, 0x2322, 0x2426, 0x2522, 0x2622, 0x2726, 0x282e, 0x292a,
    0x2a3a, 0x2b3e, 0x2c3a, 0x2d3e, 0x2e3e, 0x2f3a, 0x3026, 0x3122,
    0x3222, 0x3326, 0x3422, 0x3526, 0x3626, 0x3722, 0x382a, 0x392e,
    0x3a3e, 0x3b3a, 0x3c3e, 0x3d3a, 0x3e3a, 0x3f3e, 0x4002, 0x4106,
    0x4206, 0x4302, 0x4406, 0x4502, 0x4602, 0x4706, 0x480e, 0x490a,
    0x4a1a, 0x4b1e, 0x4c1a, 0x4d1e, 0x4e1e, 0x4f1a, 0x5006, 0x5102,
    0x5202, 0x5306, 0x5402, 0x5506, 0x5606, 0x5702, 0x580a, 0x590e,
    0x5a1e, 0x5b1a, 0x5c1e, 0x5d1a, 0x5e1a, 0x5f1e, 0x6026, 0x6122,
    0x6222, 0x6326, 0x6422, 0x6526, 0x6626, 0x6722, 0x682a, 0x692e,
    0x6a3e, 0x6b3a, 0x6c3e, 0x6d3a, 0x6e3a, 0x6f3e, 0x7022, 0x7126,
    0x7226, 0x7322, 0x7426, 0x7522, 0x7622, 0x7726, 0x782e, 0x792a,
    0x7a3a, 0x7b3e, 0x7c3a, 0x7d3e, 0x7e3e, 0x7f3a, 0x8082, 0x8186,
    0x8286, 0x8382, 0x8486, 0x8582, 0x8682, 0x8786, 0x888e, 0x898a,
    0x8a9a, 0x8b9e, 0x8c9a, 0x8d9e, 0x8e9e, 0x8f9a, 0x9086, 0x9182,
    0x9282, 0x9386, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f,
    0x3a3f, 0x3b3b, 0x3c3f, 0x3d3b, 0x3e3b, 0x3f3f, 0x4003, 0x4107,
    0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b,
    0x4a1b, 0x4b1f, 0x4c1b, 0x4d1f, 0x4e1f, 0x4f1b, 0x5007, 0x5103,
    0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f,
    0x5a1f, 0x5b1b, 0x5c1f, 0x5d1b, 0x5e1b, 0x5f1f, 0x6027, 0x6123,
    0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f,
    0x6a3f, 0x6b3b, 0x6c3f, 0x6d3b, 0x6e3b, 0x6f3f, 0x7023, 0x7127,
    0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b,
    0x7a3b, 0x7b3f, 0x7c3b, 0x7d3f, 0x7e3f, 0x7f3b, 0x8083, 0x8187,
    0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b,
    0x8a9b, 0x8b9f, 0x8c9b, 0x8d9f, 0x8e9f, 0x8f9b, 0x9087, 0x9183,
    0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f,
    0x9a9f, 0x9b9b, 0x9c9f, 0x9d9b, 0x9e9b, 0x9f9f, 0xa0a7, 0xa1a3,
    0xa2a3, 0xa3a7, 0xa4a3, 0xa5a7, 0xa6a7, 0xa7a3, 0xa8ab, 0xa9af,
    0xaabf, 0xabbb, 0xacbf, 0xadbb, 0xaebb, 0xafbf, 0xb0a3, 0xb1a7,
    0xb2a7, 0xb3a3, 0xb4a7, 0xb5a3, 0xb6a3, 0xb7a7, 0xb8af, 0xb9ab,
    0xbabb, 0xbbbf, 0xbcbb, 0xbdbf, 0xbebf, 0xbfbb, 0xc087, 0xc183,
    0xc283, 0xc387, 0xc483, 0xc587, 0xc687, 0xc783, 0xc88b, 0xc98f,
    0xca9f, 0xcb9b, 0xcc9f, 0xcd9b, 0xce9b, 0xcf9f, 0xd083, 0xd187,
    0xd287, 0xd383, 0xd487, 0xd583, 0xd683, 0xd787, 0xd88f, 0xd98b,
    0xda9b, 0xdb9f, 0xdc9b, 0xdd9f, 0xde9f, 0xdf9b, 0xe0a3, 0xe1a7,
    0xe2a7, 0xe3a3, 0xe4a7, 0xe5a3, 0xe6a3, 0xe7a7, 0xe8af, 0xe9ab,
    0xeabb, 0xebbf, 0xecbb, 0xedbf, 0xeebf, 0xefbb, 0xf0a7, 0xf1a3,
    0xf2a3, 0xf3a7, 0xf4a3, 0xf5a7, 0xf6a7, 0xf7a3, 0xf8ab, 0xf9af,
    0xfabf, 0xfbbb, 0xfcbf, 0xfdbb, 0xfebb, 0xffbf, 0x0047, 0x0103,
    0x0203, 0x0307, 0x0403, 0x0507, 0x0607, 0x0703, 0x080b, 0x090f,
    0x0a1f, 0x0b1b, 0x0c1f, 0x0d1b, 0x0e1b, 0x0f1f, 0x1003, 0x1107,
    0x1207, 0x1303, 0x1407, 0x1503, 0x1603, 0x1707, 0x180f, 0x190b,
    0x1a1b, 0x1b1f, 0x1c1b, 0x1d1f, 0x1e1f, 0x1f1b, 0x2023, 0x2127,
    0x2227, 0x2323, 0x2427, 0x2523, 0x2623, 0x2727, 0x282f, 0x292b,
    0x2a3b, 0x2b3f, 0x2c3b, 0x2d3f, 0x2e3f, 0x2f3b, 0x3027, 0x3123,
    0x3223, 0x3327, 0x3423, 0x3527, 0x3627, 0x3723, 0x382b, 0x392f,
    0x3a3f, 0x3b3b, 0x3c3f, 0x3d3b, 0x3e3b, 0x3f3f, 0x4003, 0x4107,
    0x4207, 0x4303, 0x4407, 0x4503, 0x4603, 0x4707, 0x480f, 0x490b,
    0x4a1b, 0x4b1f, 0x4c1b, 0x4d1f, 0x4e1f, 0x4f1b, 0x5007, 0x5103,
    0x5203, 0x5307, 0x5403, 0x5507, 0x5607, 0x5703, 0x580b, 0x590f,
    0x5a1f, 0x5b1b, 0x5c1f, 0x5d1b, 0x5e1b, 0x5f1f, 0x6027, 0x6123,
    0x6223, 0x6327, 0x6423, 0x6527, 0x6627, 0x6723, 0x682b, 0x692f,
    0x6a3f, 0x6b3b, 0x6c3f, 0x6d3b, 0x6e3b, 0x6f3f, 0x7023, 0x7127,
    0x7227, 0x7323, 0x7427, 0x7523, 0x7623, 0x7727, 0x782f, 0x792b,
    0x7a3b, 0x7b3f, 0x7c3b, 0x7d3f, 0x7e3f, 0x7f3b, 0x8083, 0x8187,
    0x8287, 0x8383, 0x8487, 0x8583, 0x8683, 0x8787, 0x888f, 0x898b,
    0x8a9b, 0x8b9f, 0x8c9b, 0x8d9f, 0x8e9f, 0x8f9b, 0x9087, 0x9183,
    0x9283, 0x9387, 0x9483, 0x9587, 0x9687, 0x9783, 0x988b, 0x998f
};

static const uint8_t ZSTable[256] = {
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
};

static const uint8_t PZSTable[256] = {
    0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84
};

#if 0
static const uint8_t Cycles[256] = {
     4, 10,  7,  6,  4,  4,  7,  4,  4, 11,  7,  6,  4,  4,  7,  4,
     8, 10,  7,  6,  4,  4,  7,  4, 12, 11,  7,  6,  4,  4,  7,  4,
     7, 10, 16,  6,  4,  4,  7,  4,  7, 11, 16,  6,  4,  4,  7,  4,
     7, 10, 13,  6, 11, 11, 10,  4,  7, 11, 13,  6,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     7,  7,  7,  7,  7,  7,  4,  7,  4,  4,  4,  4,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
     5, 10, 10, 10, 10, 11,  7, 11,  5, 10, 10,  4, 10, 17,  7, 11,
     5, 10, 10, 11, 10, 11,  7, 11,  5,  4, 10, 11, 10,  4,  7, 11,
     5, 10, 10, 19, 10, 11,  7, 11,  5,  4, 10,  4, 10,  4,  7, 11,
     5, 10, 10,  4, 10, 11,  7, 11,  5,  6, 10,  4, 10,  4,  7, 11,
};
#endif

static const uint8_t Cycles[256] = {
    4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
    8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4,
    7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
    7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11,
    5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
    5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
    5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11
};

static const uint8_t CyclesXX[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0, 0,
    0,14,20,10, 9, 9, 9, 0, 0,15,20,10, 9, 9, 9, 0,
    0, 0, 0, 0,23,23,19, 0, 0,15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
   19,19,19,19,19,19,19,19, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 9, 9,19, 0, 0, 0, 0, 0, 9, 9,19, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,14, 0,23, 0,15, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0
};

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcCpuZ80a::%s()", function);
}

static uint8_t default_mreq_m1_handler(XcpcCpuZ80a* self, uint16_t addr, uint8_t data)
{
    log_trace("default_mreq_m1_handler");

    return 0x00;
}

static uint8_t default_mreq_rd_handler(XcpcCpuZ80a* self, uint16_t addr, uint8_t data)
{
    log_trace("default_mreq_rd_handler");

    return 0x00;
}

static uint8_t default_mreq_wr_handler(XcpcCpuZ80a* self, uint16_t addr, uint8_t data)
{
    log_trace("default_mreq_wr_handler");

    return 0x00;
}

static uint8_t default_iorq_m1_handler(XcpcCpuZ80a* self, uint16_t addr, uint8_t data)
{
    log_trace("default_iorq_m1_handler");

    return 0x00;
}

static uint8_t default_iorq_rd_handler(XcpcCpuZ80a* self, uint16_t addr, uint8_t data)
{
    log_trace("default_iorq_rd_handler");

    return 0x00;
}

static uint8_t default_iorq_wr_handler(XcpcCpuZ80a* self, uint16_t addr, uint8_t data)
{
    log_trace("default_iorq_wr_handler");

    return 0x00;
}

XcpcCpuZ80a* xcpc_cpu_z80a_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcCpuZ80a);
}

XcpcCpuZ80a* xcpc_cpu_z80a_free(XcpcCpuZ80a* self)
{
    log_trace("free");

    return xcpc_delete(XcpcCpuZ80a, self);
}

XcpcCpuZ80a* xcpc_cpu_z80a_construct(XcpcCpuZ80a* self)
{
    log_trace("construct");

    /* clear all */ {
        (void) memset(&self->iface, 0, sizeof(XcpcCpuZ80aIface));
        (void) memset(&self->setup, 0, sizeof(XcpcCpuZ80aSetup));
        (void) memset(&self->state, 0, sizeof(XcpcCpuZ80aState));
    }
    /* initialize iface */ {
        (void) xcpc_cpu_z80a_set_iface(self, NULL);
    }
    /* reset */ {
        (void) xcpc_cpu_z80a_reset(self);
    }
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_destruct(XcpcCpuZ80a* self)
{
    log_trace("destruct");

    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_new(void)
{
    log_trace("new");

    return xcpc_cpu_z80a_construct(xcpc_cpu_z80a_alloc());
}

XcpcCpuZ80a* xcpc_cpu_z80a_delete(XcpcCpuZ80a* self)
{
    log_trace("delete");

    return xcpc_cpu_z80a_free(xcpc_cpu_z80a_destruct(self));
}

XcpcCpuZ80a* xcpc_cpu_z80a_set_iface(XcpcCpuZ80a* self, const XcpcCpuZ80aIface* iface)
{
    log_trace("set_iface");

    if(iface != NULL) {
        *(&self->iface) = *(iface);
    }
    else {
        self->iface.user_data = NULL;
        self->iface.mreq_m1   = &default_mreq_m1_handler;
        self->iface.mreq_rd   = &default_mreq_rd_handler;
        self->iface.mreq_wr   = &default_mreq_wr_handler;
        self->iface.iorq_m1   = &default_iorq_m1_handler;
        self->iface.iorq_rd   = &default_iorq_rd_handler;
        self->iface.iorq_wr   = &default_iorq_wr_handler;
    }
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_reset(XcpcCpuZ80a* self)
{
    log_trace("reset");

    /* reset state.regs */ {
        AF_R = 0;
        BC_R = 0;
        DE_R = 0;
        HL_R = 0;
        IX_R = 0;
        IY_R = 0;
        SP_R = 0;
        PC_R = 0;
        IR_R = 0;
        ST_R = 0;
    }
    /* reset state.ctrs */ {
        M_CYCLES = 0;
        T_STATES = 0;
        I_PERIOD = 0;
    }
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_clock(XcpcCpuZ80a* self)
{
    uint16_t prev_pc;
    uint8_t  last_op;
    XcpcRegister WZ;
    XcpcRegister T0;
    XcpcRegister T1;
    XcpcRegister T2;
    XcpcRegister T3;

    /* avoid unused warning */ {
        AVOID_UNUSED_WARNING(prev_pc);
        AVOID_UNUSED_WARNING(last_op);
    }
    if(I_PERIOD <= 0) {
        return self;
    }

prolog:
    prev_pc = PC_W;
    if(m_pending_nmi()) {
        m_acknowledge_nmi();
        m_push_r16(PC_W);
        m_rst_vec16(VECTOR_66H);
        m_consume(3, 11);
        goto epilog;
    }
    if(m_pending_int()) {
        m_acknowledge_int();
        m_push_r16(PC_W);
        m_rst_vec16(VECTOR_38H);
        m_consume(3, 13);
        goto epilog;
    }
    if(m_halted()) {
        m_refresh_dram();
        m_consume(1, 4);
        goto epilog;
    }
    goto fetch_opcode;

fetch_opcode:
  m_fetch_opcode();
  m_refresh_dram();
  goto execute_opcode;

execute_opcode:
  switch(last_op) {
#include "cpu-z80a-opcodes.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x23: /* INC HL            */
      self->state.regs.HL.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x26: /* LD H,n            */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x2b: /* DEC HL            */
      self->state.regs.HL.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x2e: /* LD L,n            */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x44: /* LD B,H            */
      self->state.regs.BC.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x45: /* LD B,L            */
      self->state.regs.BC.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x46: /* LD B,(HL)         */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4c: /* LD C,H            */
      self->state.regs.BC.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4d: /* LD C,L            */
      self->state.regs.BC.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4e: /* LD C,(HL)         */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x54: /* LD D,H            */
      self->state.regs.DE.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x55: /* LD D,L            */
      self->state.regs.DE.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x56: /* LD D,(HL)         */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5c: /* LD E,H            */
      self->state.regs.DE.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5d: /* LD E,L            */
      self->state.regs.DE.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5e: /* LD E,(HL)         */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x60: /* LD H,B            */
      self->state.regs.HL.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x61: /* LD H,C            */
      self->state.regs.HL.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x62: /* LD H,D            */
      self->state.regs.HL.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x63: /* LD H,E            */
      self->state.regs.HL.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x64: /* LD H,H            */
      self->state.regs.HL.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x65: /* LD H,L            */
      self->state.regs.HL.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x66: /* LD H,(HL)         */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x67: /* LD H,A            */
      self->state.regs.HL.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x68: /* LD L,B            */
      self->state.regs.HL.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x69: /* LD L,C            */
      self->state.regs.HL.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6a: /* LD L,D            */
      self->state.regs.HL.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6b: /* LD L,E            */
      self->state.regs.HL.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6c: /* LD L,H            */
      self->state.regs.HL.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6d: /* LD L,L            */
      self->state.regs.HL.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6e: /* LD L,(HL)         */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x6f: /* LD L,A            */
      self->state.regs.HL.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x70: /* LD (HL),B         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x71: /* LD (HL),C         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x72: /* LD (HL),D         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x73: /* LD (HL),E         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x74: /* LD (HL),H         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x75: /* LD (HL),L         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x76: /* HALT              */
      self->state.regs.ST.b.l |= ST_HLT;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x77: /* LD (HL),A         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7c: /* LD A,H            */
      self->state.regs.AF.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7d: /* LD A,L            */
      self->state.regs.AF.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7e: /* LD A,(HL)         */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x84: /* ADD A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x85: /* ADD A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x86: /* ADD A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8c: /* ADC A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8d: /* ADC A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8e: /* ADC A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x94: /* SUB A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x95: /* SUB A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x96: /* SUB A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9c: /* SBC A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9d: /* SBC A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9e: /* SBC A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa4: /* AND A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa5: /* AND A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa6: /* AND A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xac: /* XOR A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xad: /* XOR A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xae: /* XOR A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb4: /* OR A,H            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb5: /* OR A,L            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb6: /* OR A,(HL)         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbc: /* CP A,H            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbd: /* CP A,L            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbe: /* CP A,(HL)         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xcb:
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto fetch_cb_opcode;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xdd:
      goto fetch_dd_opcode;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xed:
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto fetch_ed_opcode;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xfd:
      goto fetch_fd_opcode;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
  }
  self->state.ctrs.t_states += Cycles[last_op];
  self->state.ctrs.i_period -= Cycles[last_op];
  goto epilog;

fetch_dd_opcode:
  m_fetch_dd_opcode();
  m_refresh_dram();
  goto execute_dd_opcode;

execute_dd_opcode:
  switch(last_op) {
#include "cpu-z80a-opcodes-dd.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x23: /* INC IX            */
      self->state.regs.IX.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x26: /* LD IXh,n          */
      self->state.regs.IX.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x2b: /* DEC IX            */
      self->state.regs.IX.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x2e: /* LD IXl,n          */
      self->state.regs.IX.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x44: /* LD B,IXh          */
      self->state.regs.BC.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x45: /* LD B,IXl          */
      self->state.regs.BC.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x46: /* LD B,(IX+d)       */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4c: /* LD C,IXh          */
      self->state.regs.BC.b.l = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4d: /* LD C,IXl          */
      self->state.regs.BC.b.l = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4e: /* LD C,(IX+d)       */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x54: /* LD D,IXh          */
      self->state.regs.DE.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x55: /* LD D,IXl          */
      self->state.regs.DE.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x56: /* LD D,(IX+d)       */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5c: /* LD E,IXh          */
      self->state.regs.DE.b.l = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5d: /* LD E,IXl          */
      self->state.regs.DE.b.l = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5e: /* LD E,(IX+d)       */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x60: /* LD IXh,B          */
      self->state.regs.IX.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x61: /* LD IXh,C          */
      self->state.regs.IX.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x62: /* LD IXh,D          */
      self->state.regs.IX.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x63: /* LD IXh,E          */
      self->state.regs.IX.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x64: /* LD IXh,IXh        */
      self->state.regs.IX.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x65: /* LD IXh,IXl        */
      self->state.regs.IX.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x66: /* LD H,(IX+d)       */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x67: /* LD IXh,A          */
      self->state.regs.IX.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x68: /* LD IXl,B          */
      self->state.regs.IX.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x69: /* LD IXl,C          */
      self->state.regs.IX.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6a: /* LD IXl,D          */
      self->state.regs.IX.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6b: /* LD IXl,E          */
      self->state.regs.IX.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6c: /* LD IXl,IXh        */
      self->state.regs.IX.b.l = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6d: /* LD IXl,IXl        */
      self->state.regs.IX.b.l = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6e: /* LD L,(IX+d)       */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x6f: /* LD IXl,A          */
      self->state.regs.IX.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x70: /* LD (IX+d),B       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x71: /* LD (IX+d),C       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x72: /* LD (IX+d),D       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x73: /* LD (IX+d),E       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x74: /* LD (IX+d),H       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x75: /* LD (IX+d),L       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x76: /* HALT              */
      self->state.regs.ST.b.l |= ST_HLT;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x77: /* LD (IX+d),A       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7c: /* LD A,IXh          */
      self->state.regs.AF.b.h = self->state.regs.IX.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7d: /* LD A,IXl          */
      self->state.regs.AF.b.h = self->state.regs.IX.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7e: /* LD A,(IX+d)       */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x84: /* ADD A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x85: /* ADD A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x86: /* ADD A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8c: /* ADC A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8d: /* ADC A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8e: /* ADC A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x94: /* SUB A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x95: /* SUB A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x96: /* SUB A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9c: /* SBC A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9d: /* SBC A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9e: /* SBC A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa4: /* AND A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa5: /* AND A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa6: /* AND A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xac: /* XOR A,IXh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xad: /* XOR A,IXl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xae: /* XOR A,(IX+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb4: /* OR A,IXh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb5: /* OR A,IXl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb6: /* OR A,(IX+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbc: /* CP A,IXh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbd: /* CP A,IXl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbe: /* CP A,(IX+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IX.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xcb:
      goto fetch_ddcb_opcode;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
  }
  self->state.ctrs.t_states += CyclesXX[last_op];
  self->state.ctrs.i_period -= CyclesXX[last_op];
  goto epilog;

fetch_fd_opcode:
  m_fetch_fd_opcode();
  m_refresh_dram();
  goto execute_fd_opcode;

execute_fd_opcode:
  switch(last_op) {
#include "cpu-z80a-opcodes-fd.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x23: /* INC IY            */
      self->state.regs.IY.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x26: /* LD IYh,n          */
      self->state.regs.IY.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x2b: /* DEC IY            */
      self->state.regs.IY.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x2e: /* LD IYl,n          */
      self->state.regs.IY.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x44: /* LD B,IYh          */
      self->state.regs.BC.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x45: /* LD B,IYl          */
      self->state.regs.BC.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x46: /* LD B,(IY+d)       */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4c: /* LD C,IYh          */
      self->state.regs.BC.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4d: /* LD C,IYl          */
      self->state.regs.BC.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4e: /* LD C,(IY+d)       */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x54: /* LD D,IYh          */
      self->state.regs.DE.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x55: /* LD D,IYl          */
      self->state.regs.DE.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x56: /* LD D,(IY+d)       */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5c: /* LD E,IYh          */
      self->state.regs.DE.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5d: /* LD E,IYl          */
      self->state.regs.DE.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5e: /* LD E,(IY+d)       */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x60: /* LD IYh,B          */
      self->state.regs.IY.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x61: /* LD IYh,C          */
      self->state.regs.IY.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x62: /* LD IYh,D          */
      self->state.regs.IY.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x63: /* LD IYh,E          */
      self->state.regs.IY.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x64: /* LD IYh,IYh        */
      self->state.regs.IY.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x65: /* LD IYh,IYl        */
      self->state.regs.IY.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x66: /* LD H,(IY+d)       */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x67: /* LD IYh,A          */
      self->state.regs.IY.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x68: /* LD IYl,B          */
      self->state.regs.IY.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x69: /* LD IYl,C          */
      self->state.regs.IY.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6a: /* LD IYl,D          */
      self->state.regs.IY.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6b: /* LD IYl,E          */
      self->state.regs.IY.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6c: /* LD IYl,IYh        */
      self->state.regs.IY.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6d: /* LD IYl,IYl        */
      self->state.regs.IY.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6e: /* LD L,(IY+d)       */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x6f: /* LD IYl,A          */
      self->state.regs.IY.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x70: /* LD (IY+d),B       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x71: /* LD (IY+d),C       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x72: /* LD (IY+d),D       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x73: /* LD (IY+d),E       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x74: /* LD (IY+d),H       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x75: /* LD (IY+d),L       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x76: /* HALT              */
      self->state.regs.ST.b.l |= ST_HLT;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x77: /* LD (IY+d),A       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7c: /* LD A,IYh          */
      self->state.regs.AF.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7d: /* LD A,IYl          */
      self->state.regs.AF.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7e: /* LD A,(IY+d)       */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x84: /* ADD A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x85: /* ADD A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x86: /* ADD A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8c: /* ADC A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8d: /* ADC A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8e: /* ADC A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x94: /* SUB A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x95: /* SUB A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x96: /* SUB A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9c: /* SBC A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9d: /* SBC A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9e: /* SBC A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa4: /* AND A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa5: /* AND A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa6: /* AND A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xac: /* XOR A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xad: /* XOR A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xae: /* XOR A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb4: /* OR A,IYh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb5: /* OR A,IYl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb6: /* OR A,(IY+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbc: /* CP A,IYh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbd: /* CP A,IYl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbe: /* CP A,(IY+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xcb:
      goto fetch_fdcb_opcode;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
  }
  self->state.ctrs.t_states += CyclesXX[last_op];
  self->state.ctrs.i_period -= CyclesXX[last_op];
  goto epilog;

fetch_cb_opcode:
    m_fetch_cb_opcode();
    m_refresh_dram();
    goto execute_cb_opcode;

execute_cb_opcode:
    switch(last_op) {
#include "cpu-z80a-opcodes-cb.inc"
        default:
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_illegal_cb();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_ed_opcode:
    m_fetch_ed_opcode();
    m_refresh_dram();
    goto execute_ed_opcode;

execute_ed_opcode:
    switch(last_op) {
#include "cpu-z80a-opcodes-ed.inc"
        default:
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_illegal_ed();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_ddcb_opcode:
    m_fetch_ddcb_offset();
    m_fetch_ddcb_opcode();
    goto execute_ddcb_opcode;

execute_ddcb_opcode:
    switch(last_op) {
#include "cpu-z80a-opcodes-ddcb.inc"
        default:
            {
                const uint32_t m_cycles = 4;
                const uint32_t t_states = 16;
                m_illegal_ddcb();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_fdcb_opcode:
    m_fetch_fdcb_offset();
    m_fetch_fdcb_opcode();
    goto execute_fdcb_opcode;

execute_fdcb_opcode:
    switch(last_op) {
#include "cpu-z80a-opcodes-fdcb.inc"
        default:
            {
                const uint32_t m_cycles = 4;
                const uint32_t t_states = 16;
                m_illegal_fdcb();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

epilog:
    if(I_PERIOD > 0) {
        goto prolog;
    }
    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_pulse_nmi(XcpcCpuZ80a* self)
{
    ST_L |= ST_NMI;

    return self;
}

XcpcCpuZ80a* xcpc_cpu_z80a_pulse_int(XcpcCpuZ80a* self)
{
    if(HAS_IFF1) {
        ST_L |= ST_INT;
    }
    return self;
}

uint8_t xcpc_cpu_z80a_get_af_h(XcpcCpuZ80a* self)
{
    return AF_H;
}

uint8_t xcpc_cpu_z80a_set_af_h(XcpcCpuZ80a* self, uint8_t data)
{
    return AF_H = data;
}

uint8_t xcpc_cpu_z80a_get_af_l(XcpcCpuZ80a* self)
{
    return AF_L;
}

uint8_t xcpc_cpu_z80a_set_af_l(XcpcCpuZ80a* self, uint8_t data)
{
    return AF_L = data;
}

uint8_t xcpc_cpu_z80a_get_bc_h(XcpcCpuZ80a* self)
{
    return BC_H;
}

uint8_t xcpc_cpu_z80a_set_bc_h(XcpcCpuZ80a* self, uint8_t data)
{
    return BC_H = data;
}

uint8_t xcpc_cpu_z80a_get_bc_l(XcpcCpuZ80a* self)
{
    return BC_L;
}

uint8_t xcpc_cpu_z80a_set_bc_l(XcpcCpuZ80a* self, uint8_t data)
{
    return BC_L = data;
}

uint8_t xcpc_cpu_z80a_get_de_h(XcpcCpuZ80a* self)
{
    return DE_H;
}

uint8_t xcpc_cpu_z80a_set_de_h(XcpcCpuZ80a* self, uint8_t data)
{
    return DE_H = data;
}

uint8_t xcpc_cpu_z80a_get_de_l(XcpcCpuZ80a* self)
{
    return DE_L;
}

uint8_t xcpc_cpu_z80a_set_de_l(XcpcCpuZ80a* self, uint8_t data)
{
    return DE_L = data;
}

uint8_t xcpc_cpu_z80a_get_hl_h(XcpcCpuZ80a* self)
{
    return HL_H;
}

uint8_t xcpc_cpu_z80a_set_hl_h(XcpcCpuZ80a* self, uint8_t data)
{
    return HL_H = data;
}

uint8_t xcpc_cpu_z80a_get_hl_l(XcpcCpuZ80a* self)
{
    return HL_L;
}

uint8_t xcpc_cpu_z80a_set_hl_l(XcpcCpuZ80a* self, uint8_t data)
{
    return HL_L = data;
}

uint8_t xcpc_cpu_z80a_get_ix_h(XcpcCpuZ80a* self)
{
    return IX_H;
}

uint8_t xcpc_cpu_z80a_set_ix_h(XcpcCpuZ80a* self, uint8_t data)
{
    return IX_H = data;
}

uint8_t xcpc_cpu_z80a_get_ix_l(XcpcCpuZ80a* self)
{
    return IX_L;
}

uint8_t xcpc_cpu_z80a_set_ix_l(XcpcCpuZ80a* self, uint8_t data)
{
    return IX_L = data;
}

uint8_t xcpc_cpu_z80a_get_iy_h(XcpcCpuZ80a* self)
{
    return IY_H;
}

uint8_t xcpc_cpu_z80a_set_iy_h(XcpcCpuZ80a* self, uint8_t data)
{
    return IY_H = data;
}

uint8_t xcpc_cpu_z80a_get_iy_l(XcpcCpuZ80a* self)
{
    return IY_L;
}

uint8_t xcpc_cpu_z80a_set_iy_l(XcpcCpuZ80a* self, uint8_t data)
{
    return IY_L = data;
}

uint8_t xcpc_cpu_z80a_get_sp_h(XcpcCpuZ80a* self)
{
    return SP_H;
}

uint8_t xcpc_cpu_z80a_set_sp_h(XcpcCpuZ80a* self, uint8_t data)
{
    return SP_H = data;
}

uint8_t xcpc_cpu_z80a_get_sp_l(XcpcCpuZ80a* self)
{
    return SP_L;
}

uint8_t xcpc_cpu_z80a_set_sp_l(XcpcCpuZ80a* self, uint8_t data)
{
    return SP_L = data;
}

uint8_t xcpc_cpu_z80a_get_pc_h(XcpcCpuZ80a* self)
{
    return PC_H;
}

uint8_t xcpc_cpu_z80a_set_pc_h(XcpcCpuZ80a* self, uint8_t data)
{
    return PC_H = data;
}

uint8_t xcpc_cpu_z80a_get_pc_l(XcpcCpuZ80a* self)
{
    return PC_L;
}

uint8_t xcpc_cpu_z80a_set_pc_l(XcpcCpuZ80a* self, uint8_t data)
{
    return PC_L = data;
}

uint8_t xcpc_cpu_z80a_get_ir_h(XcpcCpuZ80a* self)
{
    return IR_H;
}

uint8_t xcpc_cpu_z80a_set_ir_h(XcpcCpuZ80a* self, uint8_t data)
{
    return IR_H = data;
}

uint8_t xcpc_cpu_z80a_get_ir_l(XcpcCpuZ80a* self)
{
    return IR_L;
}

uint8_t xcpc_cpu_z80a_set_ir_l(XcpcCpuZ80a* self, uint8_t data)
{
    return IR_L = data;
}

uint8_t xcpc_cpu_z80a_get_af_x(XcpcCpuZ80a* self)
{
    return AF_X;
}

uint8_t xcpc_cpu_z80a_set_af_x(XcpcCpuZ80a* self, uint8_t data)
{
    return AF_X = data;
}

uint8_t xcpc_cpu_z80a_get_af_y(XcpcCpuZ80a* self)
{
    return AF_Y;
}

uint8_t xcpc_cpu_z80a_set_af_y(XcpcCpuZ80a* self, uint8_t data)
{
    return AF_Y = data;
}

uint8_t xcpc_cpu_z80a_get_bc_x(XcpcCpuZ80a* self)
{
    return BC_X;
}

uint8_t xcpc_cpu_z80a_set_bc_x(XcpcCpuZ80a* self, uint8_t data)
{
    return BC_X = data;
}

uint8_t xcpc_cpu_z80a_get_bc_y(XcpcCpuZ80a* self)
{
    return BC_Y;
}

uint8_t xcpc_cpu_z80a_set_bc_y(XcpcCpuZ80a* self, uint8_t data)
{
    return BC_Y = data;
}

uint8_t xcpc_cpu_z80a_get_de_x(XcpcCpuZ80a* self)
{
    return DE_X;
}

uint8_t xcpc_cpu_z80a_set_de_x(XcpcCpuZ80a* self, uint8_t data)
{
    return DE_X = data;
}

uint8_t xcpc_cpu_z80a_get_de_y(XcpcCpuZ80a* self)
{
    return DE_Y;
}

uint8_t xcpc_cpu_z80a_set_de_y(XcpcCpuZ80a* self, uint8_t data)
{
    return DE_Y = data;
}

uint8_t xcpc_cpu_z80a_get_hl_x(XcpcCpuZ80a* self)
{
    return HL_X;
}

uint8_t xcpc_cpu_z80a_set_hl_x(XcpcCpuZ80a* self, uint8_t data)
{
    return HL_X = data;
}

uint8_t xcpc_cpu_z80a_get_hl_y(XcpcCpuZ80a* self)
{
    return HL_Y;
}

uint8_t xcpc_cpu_z80a_set_hl_y(XcpcCpuZ80a* self, uint8_t data)
{
    return HL_Y = data;
}

uint8_t xcpc_cpu_z80a_get_im(XcpcCpuZ80a* self)
{
    const uint8_t im = (HAS_IM1 ? 1 : 0)
                     | (HAS_IM2 ? 2 : 0)
                     ;
    return im;
}

uint8_t xcpc_cpu_z80a_set_im(XcpcCpuZ80a* self, uint8_t im)
{
    switch(im) {
        case 1:
            SET_IM1();
            CLR_IM2();
            break;
        case 2:
            CLR_IM1();
            SET_IM2();
            break;
        case 3:
            SET_IM1();
            SET_IM2();
            break;
        default:
            CLR_IM1();
            CLR_IM2();
            break;
    }
    return im;
}

uint8_t xcpc_cpu_z80a_get_iff1(XcpcCpuZ80a* self)
{
    const uint8_t iff1 = (HAS_IFF1 ? 1 : 0);

    return iff1;
}

uint8_t xcpc_cpu_z80a_set_iff1(XcpcCpuZ80a* self, uint8_t iff1)
{
    if(iff1 != 0) {
        SET_IFF1();
    }
    else {
        CLR_IFF1();
    }
    return iff1;
}

uint8_t xcpc_cpu_z80a_get_iff2(XcpcCpuZ80a* self)
{
    const uint8_t iff2 = (HAS_IFF2 ? 1 : 0);

    return iff2;
}

uint8_t xcpc_cpu_z80a_set_iff2(XcpcCpuZ80a* self, uint8_t iff2)
{
    if(iff2 != 0) {
        SET_IFF2();
    }
    else {
        CLR_IFF2();
    }
    return iff2;
}
