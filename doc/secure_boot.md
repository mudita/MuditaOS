# Secure Boot

The RT1051 processor features a Secure Boot mechanism in which a signed binary
image of the program can be authenticated against a public key stored
in the One-Time-Programmable (OTP) memory of the device also referred to as *eFuses*.

Secure Boot is meant be used in production and as long as you don't use a special CMake
configuration, you don't need to concern yourself with it.

For more information regarding Secure Boot, please refer the our internal documents:

* *Secure boot on the RT1051 MCU for Mudita Pure. A technical overview*

and to the official NXP documentation:

* *Security Reference Manual for the iMX RT1050 Processor, Rev. 2* - not publically
available on NXP site. If you're one of Mudita employees, please ask on Slack
to share it with you.
* *AN12681. How to use HAB secure boot in iMX RT10xx*

## Requirements

In order to enable Secure Boot on the device, you need the following:

* A signed bootloader (ecoboot) in a version which supports Secure Boot flashed
into the device. See the documentation in the ecoboot repository.
* A set of keys in order to sign the program image and provision the device.
Refer to [Generating keys](#generating-keys).
* An SRK hash (public key hash) written into the OTP memory and the *Closed* security
configuration fuse (bit) burned. See [Device provisioning](#device-provisioning).
* A signed MuditaOS image loaded into the device.
See [Signing MuditaOS image](#signing-muditaos-image).

## Generating keys

> The steps presented here are for generating keys for testing purposes only.

The keys used for signing and verifying the program image follow a certain PKI structure:

* SRK key - the root key of other keys. It's used for verifying the authenticity
of its subordinate keys. Its hash (called *SRK hash*) is burned into OTP memory
of the device.
* CSF key - subordinate key of SRK. Used for authenticating the CSF commands
presents in the image.
* IMG key - subordinate key of SRK. Used for authenticating the actual software
part of the image.

Actually, the SRK hash may be computed over *SRK table* which contains multiple SRKs.
This feature allow for revocation of a particular SRK in case it gets compromised.

For details regarding the PKI and key generation, refer to the *Code-Signing Tool
User’s Guide, Rev. 3.1.0*. 

The keys required for Secure Boot can be generated using the *MCUXpresso Secure
Provisioning Tool*, an NXP utility with a graphical user interface. Alternatively,
there is a set of scripts for key generation provided with the *NXP Code-Signing
Tool (CST)*. These scripts utilize OpenSSL and offer more control when compared
to the aforementioned graphical tool.

MCUXpresso Secure Provisioning Tool use the CST scripts underneath and the CST
package can be find among its files (in the `/opt/nxp/MCUX_Provi_v2.1/bin/tools/cst`
directory on Linux). The CST utility is also available as a separate download
on the NXP site.

## Device provisioning

In this context, provisioning involves writing the SRK hash into OTP memory and
burning the Closed security configuration fuse. Without performing these,
the ecoboot bootloader supporting Secure Boot will skip the image authentication
and perform a straightforward boot. This policy has been implemented so that the
Secure Boot feature won't interfere with the development of MuditaOS.

According to the documentation, the OTP memory can be programmed using the
NXP Flashloader program. When loaded into RAM using Serial Downloader boot mode,
it receives commands from the SDPHost tool and performs appropriate writes. However,
during testing it failed to run properly when loaded using the Serial Downloader
boot mode. This behaviour was not thoroughly investigated so the Flashloader
may yet happen to a viable option for eFuse programming.

During testing, eventually a simple program based on the *D1_Flashloader* (can be found on 
Mudita's GitHub) was used. It burned appropriate fuses using hardcoded values
upon running it on the MCU.

> It's important that when burning the SRK hash into eFuses, each four bytes of the
> SRK hash must be reversed.

## Signing MuditaOS image

> Signing the program image to be used with Secure Boot actually involves more
> than appending a cryptographic signature to the binary. A secure bootable image
> consist of multiple components like IVT, boot data, CSF, certificates and signatures
> besides the program itself. Refer to *Security Reference Manual for the iMX RT1050
> Processor, Rev. 2* for more information.

Image signing is optionally performed by CMake as one of the last steps of the build
process. In order to perform signing, the following CMake cache variables must be set:
* `ENABLE_SECURE_BOOT` - CMake option which enables image signing, set it to `ON`
* `CST_PATH` (optional) - a path to the NXP CST utility. If not specified, it is
assumed that CST is available from PATH.
* `ELFTOSB_PATH` (optional) - a path to the NXP elftosb utility. If not specified,
it is assumend that elftosb is available from PATH.
* `SRK_TABLE` - a full path to the SRK table binary file
* `SRK_INDEX` - the index in the SRK table of the SRK to be used for authentication.
Zero by default.
* `CSF_KEY` - a full path to the CSF key in PEM format
* `IMG_KEY` - a full path to the IMG key in PEM format

As you can see, in order to prepare a binary for Secure Boot, two utilities are
required: the NXP elftosb and NXP Code-Signing Tool (CST). These are used by the
MCUXpresso Secure Provisioning Tool so they can be found in its installation
directory (`/opt/nxp/MCUX_Provi_v2.1/bin/tools/cst` on Linux).

The latest version of the utility can be downloaded from the following locations:

[CST tool](https://www.nxp.com/webapp/sps/download/license.jsp?colCode=IMX_CST_TOOL)

[ELFTOSB](https://www.nxp.com/webapp/sps/download/license.jsp?colCode=elftosb_5.1.19)

You can set the required CMake variables when configuring the project using the
`configure.sh` script:
``` bash
./configure.sh rt1051 RelWithDebInfo \
-DENABLE_SECURE_BOOT=ON \
-DCST_PATH="/opt/nxp/MCUX_Provi_v2.1/bin/tools/cst/linux64/bin" \
-DELFTOSB_PATH="/opt/nxp/MCUX_Provi_v2.1/bin/tools/elftosb/linux/amd64" \
-DSRK_TABLE="/home/SRK_hash.bin" \
-DCSF_KEY="/home/CSF1_1_sha256_2048_65537_v3_usr_crt.pem" \
-DIMG_KEY="/home/IMG1_1_sha256_2048_65537_v3_usr_crt.pem"
```