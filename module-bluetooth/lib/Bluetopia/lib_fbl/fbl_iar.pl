#!/usr/bin/perl
#
# File:        fbl_iar.pl
# Author:      Tim Cook
# Changed on 14 june 2015- Add the processor build option for the library creation.
# Description: Builds Flexible Build Library for IAR
# Usage:       fbl_iar.pl [--sppserver] [--sppclient] [--sdpserver] [--sdpclient]
#                         [--lemaster] [--leslave] [--gattserver] [--gattclient]
#                         [--hidhost]  [--hiddevice] [--sco] [--goepserver] [--goepclient] 
#                         [--mapserver] [--mapclient] [--pbapserver] [--pbapclient]
#                         [--help]
#
use strict;
use Getopt::Long;

my $ObjectsNameSuffix = shift or die "\n Error! Please provide processor build option, e.g. 16_M3, 16_M4, 16_M4.fp_HW_FP, 16_M7.fp.dp, 16_M7.fp.sp, 16_M7, 32_M3, 32_M4, 32_M4.fp_HW_FP, 32_M7.fp.dp, 32_M7.fp.sp, 32_M7 . \n";
if($ObjectsNameSuffix eq "16_M3") {
	print "\n Building libraries for ARM Cortex M3, wchar=16bit \n";
}
elsif($ObjectsNameSuffix eq "16_M4") {
	print "\n Building libraries for ARM Cortex M4, wchar=16bit \n";
}
elsif($ObjectsNameSuffix eq "16_M4.fp_HW_FP") {
	print "\n Building libraries for ARM Cortex M4, wchar=16bit with HW Floating point \n";
}
elsif($ObjectsNameSuffix eq "16_M7.fp.dp") {
	print "\n Building libraries for ARM Cortex M7.fp.dp, wchar=16bit with double precision HW Floating point \n";
}
elsif($ObjectsNameSuffix eq "16_M7.fp.sp") {
	print "\n Building libraries for ARM Cortex M7.fp.sp, wchar=16bit with single precision HW Floating point \n";
}
elsif($ObjectsNameSuffix eq "16_M7") {
	print "\n Building libraries for ARM Cortex M7, wchar=16bit \n";
}
elsif($ObjectsNameSuffix eq "32_M3") {
	print "\n Building libraries for ARM Cortex M3, wchar=32bit \n";
}
elsif($ObjectsNameSuffix eq "32_M4") {
	print "\n Building libraries for ARM Cortex M4, wchar=32bit \n";
}
elsif($ObjectsNameSuffix eq "32_M4.fp_HW_FP") {
	print "\n Building libraries for ARM Cortex M4, wchar=32bit with HW Floating point \n";
}
elsif($ObjectsNameSuffix eq "32_M7.fp.dp") {
	print "\n Building libraries for ARM Cortex M7.fp.dp, wchar=32bit with double precision HW Floating point \n";
}
elsif($ObjectsNameSuffix eq "32_M7.fp.sp") {
	print "\n Building libraries for ARM Cortex M7.fp.sp, wchar=32bit with single precision HW Floating point \n";
}
elsif($ObjectsNameSuffix eq "32_M7") {
	print "\n Building libraries for ARM Cortex M7, wchar=32bit \n";
}
elsif($ObjectsNameSuffix eq "--help") {
	print "\n Printing help... \n";
}
else {
	print "\n Error! Please provide valid processor build option, e.g. 16_M3, 16_M4, 16_M4.fp_HW_FP, 16_M7.fp.dp, 16_M7.fp.sp, 16_M7, 32_M3, 32_M4, 32_M4.fp_HW_FP, 32_M7.fp.dp, 32_M7.fp.sp, 32_M7 . \n";
	exit(0);
}

if($ObjectsNameSuffix ne "--help") {
	print "\n Building libraries from objects with name suffix= objectName_$ObjectsNameSuffix \n";
}

my $SPPServer;
my $SPPClient;
my $GOEPServer;
my $GOEPClient;
my $SDPServer;
my $SDPClient;
my $LEMaster;
my $LESlave;
my $GATTServer;
my $GATTClient;
my $MAPServer;
my $MAPClient;
my $PBAPServer;
my $PBAPClient;
my $HIDHost;
my $HIDDevice;
my $SCO;
my @Objects;
my $BluetopiaLibrary = "Bluetopia_$ObjectsNameSuffix.a";
my @GATTObjects;
my $GATTLibrary = "SS1BTGAT_$ObjectsNameSuffix.a";
my @MAPObjects;
my $MAPLibrary = "SS1BTMAP_$ObjectsNameSuffix.a";
my @PBAPObjects;
my $PBAPLibrary = "SS1BTPBA_$ObjectsNameSuffix.a";
my @HIDObjects;
my $HIDLibrary  = "SS1BTHID_$ObjectsNameSuffix.a";
my $BuildOutput;
my $Help;
my $Folder;
sub BuildLibraries;

print "\n";
print "############################################################################\n";
print "Running Flexible Build Library Utility.  IAR Archiver for ARM (iarchive) must be in the \npath variable.\n";
print "############################################################################\n";
print "\n";

#-------------------------------------------------------------------------
# Parse the command line switches
#-------------------------------------------------------------------------
GetOptions('sppserver'  => \$SPPServer,
           'sppclient'  => \$SPPClient,
           'sdpserver'  => \$SDPServer,
           'sdpclient'  => \$SDPClient,
           'lemaster'   => \$LEMaster,
           'leslave'    => \$LESlave,
           'gattserver' => \$GATTServer,
           'gattclient' => \$GATTClient,
           'mapserver'  => \$MAPServer,
           'mapclient'  => \$MAPClient,
           'pbapserver' => \$PBAPServer,
           'pbapclient' => \$PBAPClient,
           'hidhost'    => \$HIDHost,
           'hiddevice'  => \$HIDDevice,
           'sco'        => \$SCO,
           'help'       => \$Help
           );

#-------------------------------------------------------------------------
# Display Help string if requested
#-------------------------------------------------------------------------
if($Help)
{
print <<HELP;
   fbl_iar.pl options (Note all options are optional):
      --sppserver  - Support SPP Server Role
      --sppclient  - Support SPP Client Role
      --goepserver - Support GOEP Server Role
      --goepclient - Support GOEP Client Role
      --sdpserver  - Support SDP Server Role
      --sdpclient  - Support SDP Client Role
      --lemaster   - Support LE Master Role
      --leslave    - Support LE Slave Role
      --gattserver - Support GATT Server Role
      --gattclient - Support GATT Client Role
      --mapserver  - Support MAP Server Role
      --mapclient  - Support MAP Client Role
      --pbapserver - Support PBAP Server Role
      --pbapclient - Support PBAP Client Role
      --hidhost    - Support HID Host Role
      --hiddevice  - Support HID Device Role
      --sco        - Support SCO (Audio)
HELP

   exit(0);
}

# Build the RTOS version of the libraries
print "Threaded Libraries\n";
print "-" x 80;
print "\n";
BuildLibraries("FreeRTOS");

exit(0);


#-------------------------------------------------------------------------------
# Subroutine: BuildLibraries
# Arguments:
#    Directory Name - Name of directory to get objects (NoOS or RTOS)
# Descriptions:  Builds Flexible Build Library to use specifications.
#-------------------------------------------------------------------------------
sub BuildLibraries
{
   my $Directory = shift or die "Usage";

   @Objects     = ();
   @GATTObjects = ();
   @HIDObjects  = ();
   @MAPObjects  = ();
   @PBAPObjects = ();

   #-------------------------------------------------------------------------
   # Note if we have MAP support we need to also include the appropriate
   # GOEP support.
   #-------------------------------------------------------------------------
   if(($MAPServer == 1) || ($MAPClient == 1))
   {
      $GOEPServer = 1;
      $GOEPClient = 1;
   }

   #-------------------------------------------------------------------------
   # Note if we have PBAP support we need to also include the appropriate
   # GOEP support.
   #-------------------------------------------------------------------------
   if($PBAPServer == 1)
   {
      $GOEPServer = 1;
   }

   if($PBAPClient == 1)
   {
      $GOEPClient = 1;
   }

   #-------------------------------------------------------------------------
   # Note if we have GOEP support we need to also include the appropriate
   # SPP support.
   #-------------------------------------------------------------------------
   if($GOEPServer == 1)
   {
      $SPPServer = 1;
   }

   if($GOEPClient == 1)
   {
      $SPPClient = 1;
   }

   #-------------------------------------------------------------------------
   # Note if we are including SPP Server support we must have SDP Server
   # support.
   #-------------------------------------------------------------------------
   if($SPPServer == 1)
   {
      $SDPServer = 1;
   }

   #-------------------------------------------------------------------------
   # Add the common objects
   #-------------------------------------------------------------------------
   push(@Objects, "Objects\\$Directory\\HCIDrv_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\HCILL_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\HCIComm_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\TWUART_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\BSC_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\BTPSC_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\BTPSTMR_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\OTP_$ObjectsNameSuffix.o");
   # push(@Objects, "Objects\\$Directory\\HCID_$ObjectsNameSuffix.o");

   #-------------------------------------------------------------------------
   # Determine which SPP and RFCOMM objects are to be included
   #-------------------------------------------------------------------------
   if(($SPPServer == 1) && ($SPPClient == 1))
   {
   #   print "Including SPP Server and Client support.\n";
      print "SPP Support:   Server/Client.\n";

      push(@Objects, "Objects\\$Directory\\RFCOMM_SC_$ObjectsNameSuffix.o");
      push(@Objects, "Objects\\$Directory\\SPP_SC_$ObjectsNameSuffix.o");
   }
   elsif(($SPPServer == 1) && ($SPPClient == 0))
   {
   #   print "Including SPP Server support.\n";
      print "SPP Support:   Server.\n";

      push(@Objects, "Objects\\$Directory\\RFCOMM_S_$ObjectsNameSuffix.o");
      push(@Objects, "Objects\\$Directory\\SPP_S_$ObjectsNameSuffix.o");
   }
   elsif(($SPPServer == 0) && ($SPPClient == 1))
   {
   #   print "Including SPP Client support.\n";
      print "SPP Support:   Client.\n";

      push(@Objects, "Objects\\$Directory\\RFCOMM_C_$ObjectsNameSuffix.o");
      push(@Objects, "Objects\\$Directory\\SPP_C_$ObjectsNameSuffix.o");
   }
   else
   {
   #   print "Not including SPP support.\n";
      print "SPP Support:   None.\n";

      push(@Objects, "Objects\\$Directory\\RFCOMM_Empty_$ObjectsNameSuffix.o");
      push(@Objects, "Objects\\$Directory\\SPP_Empty_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which GOEP objects are to be included
   #-------------------------------------------------------------------------
   if(($GOEPServer == 1) && ($GOEPClient == 1))
   {
   #   print "Including GOEP Server and Client support.\n";
      print "GOEP Support:  Server/Client.\n";

      push(@Objects, "Objects\\$Directory\\GOEP_SC_$ObjectsNameSuffix.o");
   }
   elsif(($GOEPServer == 1) && ($GOEPClient == 0))
   {
   #   print "Including GOEP Server support.\n";
      print "GOEP Support:  Server.\n";

      push(@Objects, "Objects\\$Directory\\GOEP_S_$ObjectsNameSuffix.o");
   }
   elsif(($GOEPServer == 0) && ($GOEPClient == 1))
   {
   #   print "Including GOEP Client support.\n";
      print "GOEP Support:  Client.\n";

      push(@Objects, "Objects\\$Directory\\GOEP_C_$ObjectsNameSuffix.o");
   }
   else
   {
   #   print "Not including GOEP support.\n";
      print "GOEP Support:  None.\n";

      push(@Objects, "Objects\\$Directory\\GOEP_Empty_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which SDP Object is to be included
   #-------------------------------------------------------------------------
   if($SDPClient == 1)
   {
   #   print "Including SDP Server and Client support.\n";
      print "SDP Support:   Server/Client.\n";

      push(@Objects, "Objects\\$Directory\\SDP_SC_$ObjectsNameSuffix.o");
   }
   else
   {
   #   print "Including SDP Server support.\n";
      print "SDP Support:   Server.\n";

      push(@Objects, "Objects\\$Directory\\SDP_S_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which HCI Object to include
   #-------------------------------------------------------------------------
   if(($SCO == 1) && ($LEMaster == 1) && ($LESlave == 1))
   {
   #   print "Including SCO and Low Energy Master and Slave Role support in HCI.\n";
      print "HCI Support:   SCO,LE Master/Slave.\n";

      push(@Objects, "Objects\\$Directory\\HCI_SCO_MS_$ObjectsNameSuffix.o");
   }
   elsif(($SCO == 1) && ($LEMaster == 1) && ($LESlave == 0))
   {
   #   print "Including SCO and Low Energy Master Role support in HCI.\n";
      print "HCI Support:   SCO,LE Master.\n";

      push(@Objects, "Objects\\$Directory\\HCI_SCO_M_$ObjectsNameSuffix.o");
   }
   elsif(($SCO == 1) && ($LEMaster == 0) && ($LESlave == 1))
   {
   #   print "Including SCO and Low Energy Slave Role support in HCI.\n";
      print "HCI Support:   SCO,LE Slave.\n";

      push(@Objects, "Objects\\$Directory\\HCI_SCO_S_$ObjectsNameSuffix.o");
   }
   elsif(($SCO == 1) && ($LEMaster == 0) && ($LESlave == 0))
   {
   #   print "Including SCO support in HCI.\n";
      print "HCI Support:   SCO.\n";

      push(@Objects, "Objects\\$Directory\\HCI_SCO_$ObjectsNameSuffix.o");
   }
   elsif(($SCO == 0) && ($LEMaster == 1) && ($LESlave == 1))
   {
   #   print "Including Low Energy Master and Slave Role support in HCI.\n";
      print "HCI Support:   LE Master/Slave.\n";

      push(@Objects, "Objects\\$Directory\\HCI_NOSCO_MS_$ObjectsNameSuffix.o");
   }
   elsif(($SCO == 0) && ($LEMaster == 1) && ($LESlave == 0))
   {
   #   print "Including Low Energy Master Role support in HCI.\n";
      print "HCI Support:   LE Master.\n";

      push(@Objects, "Objects\\$Directory\\HCI_NOSCO_M_$ObjectsNameSuffix.o");
   }
   elsif(($SCO == 0) && ($LEMaster == 0) && ($LESlave == 1))
   {
   #   print "Including Low Energy Slave Role support in HCI.\n";
      print "HCI Support:   LE Slave.\n";

      push(@Objects, "Objects\\$Directory\\HCI_NOSCO_S_$ObjectsNameSuffix.o");
   }
   else
   {
   #   print "Including HCI with no SCO or LE support.\n";
      print "HCI Support:   Standard HCI Only.\n";

      push(@Objects, "Objects\\$Directory\\HCI_NOSCO_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which ECDH Objects to include
   #-------------------------------------------------------------------------
   push(@Objects, "Objects\\$Directory\\add_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\AESCMAC_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\divby2_nist192_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\incompadd_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\incompsub_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\init_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\inverse_nist_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\jacobianpointadd_nist_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\jacobpointdouble_nist_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\jacobpointmul_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\key_agreement_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\key_generate_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\m_arypointmul_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\mod_add_nist192_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\mod_multiply_nist192_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\mod_square_nist192_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\mod_sub_nist192_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\mulby2_nist192_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\P0Prime_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\public_key_validation_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\rce_config_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\rct_config_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\rct_curves_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\rctUtility_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\remainder_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\specgfp_init_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\specgfp_reduc_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\subtract_$ObjectsNameSuffix.o");
   push(@Objects, "Objects\\$Directory\\ECDHCALC_$ObjectsNameSuffix.o");
   
   #-------------------------------------------------------------------------
   # Determine which GAP Object to include
   #-------------------------------------------------------------------------
   if(($LEMaster == 1) || ($LESlave == 1))
   {
      push(@Objects, "Objects\\$Directory\\GAP_LE_$ObjectsNameSuffix.o");
   }
   else
   {
      push(@Objects, "Objects\\$Directory\\GAP_NoLE_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which GAPLE Object to include
   #-------------------------------------------------------------------------
   if(($LEMaster == 1) && ($LESlave == 1))
   {
   #   print "Including Low Energy Master and Slave Role support.\n";
      print "GAP Support:   BR/EDR, LE Master/Slave.\n";

      push(@Objects, "Objects\\$Directory\\GAPLE_MS_$ObjectsNameSuffix.o");
   }
   elsif(($LEMaster == 1) && ($LESlave == 0))
   {
   #   print "Including Low Energy Master Role support.\n";
      print "GAP Support:   BR/EDR, LE Master.\n";

      push(@Objects, "Objects\\$Directory\\GAPLE_M_$ObjectsNameSuffix.o");
   }
   elsif(($LEMaster == 0) && ($LESlave == 1))
   {
   #   print "Including Low Energy Slave Role support.\n";
      print "GAP Support:   BR/EDR, LE Slave.\n";

      push(@Objects, "Objects\\$Directory\\GAPLE_S_$ObjectsNameSuffix.o");
   }
   else
   {
      print "GAP Support:   BR/EDR.\n";

      push(@Objects, "Objects\\$Directory\\GAPLE_Empty_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which SCO Object to include
   #-------------------------------------------------------------------------
   if($SCO)
   {
   #   print "Including SCO support.\n";
      print "SCO Support:   Yes.\n";

      push(@Objects, "Objects\\$Directory\\SCO_$ObjectsNameSuffix.o");
   }
   else
   {
   #   print "Not including SCO support.\n";
      print "SCO Support:   No.\n";

      push(@Objects, "Objects\\$Directory\\SCO_Empty_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which L2CAP Object to include
   #-------------------------------------------------------------------------
   if(($LEMaster == 1) || ($LESlave == 1))
   {
      print "L2CAP Support: BR/EDR, LE.\n";

      push(@Objects, "Objects\\$Directory\\L2CAP_LE_$ObjectsNameSuffix.o");
   }
   else
   {
      print "L2CAP Support: BR/EDR.\n";

      push(@Objects, "Objects\\$Directory\\L2CAP_NoLE_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which GATT Object to include
   #-------------------------------------------------------------------------
   if(($GATTServer == 1) && ($GATTClient == 1))
   {
   #   print "Including GATT Server and Client support.\n";
      print "GATT Support:  Server/Client.\n";

      push(@GATTObjects, "Objects\\$Directory\\GATT_SC_$ObjectsNameSuffix.o");
   }
   elsif(($GATTServer == 1) && ($GATTClient == 0))
   {
   #   print "Including GATT Server support.\n";
      print "GATT Support:  Server.\n";

      push(@GATTObjects, "Objects\\$Directory\\GATT_S_$ObjectsNameSuffix.o");
   }
   elsif(($GATTServer == 0) && ($GATTClient == 1))
   {
   #   print "Including GATT Client support.\n";
      print "GATT Support:  Client.\n";

      push(@GATTObjects, "Objects\\$Directory\\GATT_C_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which HID Object to include
   #-------------------------------------------------------------------------
   if(($HIDHost == 1) && ($HIDDevice == 1))
   {
      print "HID Support:   Host/Device.\n";

      push(@HIDObjects, "Objects\\$Directory\\HID_HD_$ObjectsNameSuffix.o");
   }
   elsif(($HIDHost == 1) && ($HIDDevice == 0))
   {
      print "HID Support:   Host.\n";

      push(@HIDObjects, "Objects\\$Directory\\HID_H_$ObjectsNameSuffix.o");
   }
   elsif(($HIDHost == 0) && ($HIDDevice == 1))
   {
      print "HID Support:   Device.\n";

      push(@HIDObjects, "Objects\\$Directory\\HID_D_$ObjectsNameSuffix.o");
   }

   #-------------------------------------------------------------------------
   # Determine which MAP Object to include
   #-------------------------------------------------------------------------
   #if(($MAPServer == 1) && ($MAPClient == 1))
   #{
   ##   print "Including MAP Server and Client support.\n";
   #   print "MAP Support:   Server/Client.\n";
   #
   #   push(@MAPObjects, "Objects\\$Directory\\MAP_SC_$ObjectsNameSuffix.o");
   #}
   #elsif(($MAPServer == 1) && ($MAPClient == 0))
   #{
   ##   print "Including MAP Server support.\n";
   #   print "MAP Support:   Server.\n";
   #
   #   push(@MAPObjects, "Objects\\$Directory\\MAP_S_$ObjectsNameSuffix.o");
   #}
   #elsif(($MAPServer == 0) && ($MAPClient == 1))
   #{
   ##   print "Including MAP Client support.\n";
   #   print "MAP Support:   Client.\n";
   #
   #   push(@MAPObjects, "Objects\\$Directory\\MAP_C_$ObjectsNameSuffix.o");
   #}

   #-------------------------------------------------------------------------
   # Determine which PBAP Object to include
   #-------------------------------------------------------------------------
   #if(($PBAPServer == 1) && ($PBAPClient == 1))
   #{
   ##   print "Including PBAP Server and Client support.\n";
   #   print "PBAP Support:  Server/Client.\n";
   #
   #   push(@PBAPObjects, "Objects\\$Directory\\PBAP_SC_$ObjectsNameSuffix.o");
   #}
   #elsif(($PBAPServer == 1) && ($PBAPClient == 0))
   #{
   ##   print "Including PBAP Server support.\n";
   #   print "PBAP Support:  Server.\n";
   #
   #   push(@PBAPObjects, "Objects\\$Directory\\PBAP_S_$ObjectsNameSuffix.o");
   #}
   #elsif(($PBAPServer == 0) && ($PBAPClient == 1))
   #{
   ##   print "Including PBAP Client support.\n";
   #   print "PBAP Support:  Client.\n";
   #
   #   push(@PBAPObjects, "Objects\\$Directory\\PBAP_C_$ObjectsNameSuffix.o");
   #}

   #-------------------------------------------------------------------------
   # Create the library with the specified files
   #-------------------------------------------------------------------------
   unlink($BluetopiaLibrary);
   $BuildOutput = `iarchive --create @Objects -o $BluetopiaLibrary`;
   print $BuildOutput;

   $Folder = "generated_libs";
   mkdir($Folder, 0700) unless (-d $Folder);
   $Folder = "generated_libs/lib";
   mkdir($Folder, 0700) unless (-d $Folder);
   $Folder = "generated_libs/lib/ewarm";
   mkdir($Folder, 0700) unless (-d $Folder);
   $Folder = "generated_libs/lib/ewarm/$Directory";
   mkdir($Folder, 0700) unless (-d $Folder);

   $Folder = "generated_libs\\lib\\ewarm\\$Directory\\";

   `move $BluetopiaLibrary $Folder`;

   #-------------------------------------------------------------------------
   # Create the GATT library with the specified files if requested
   #-------------------------------------------------------------------------
   if(scalar(@GATTObjects) > 0)
   {
      unlink($GATTLibrary);
      $BuildOutput = `iarchive --create @GATTObjects -o $GATTLibrary`;

      print $BuildOutput;

      $Folder = "generated_libs/profiles/";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/gatt";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/gatt/lib";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/gatt/lib/ewarm";
      mkdir($Folder, 0700) unless (-d $Folder);

      $Folder = "generated_libs\\profiles\\gatt\\lib\\ewarm\\";

      `move $GATTLibrary $Folder`;
   }

   #-------------------------------------------------------------------------
   # Create the HID library with the specified files if requested
   #-------------------------------------------------------------------------
   if(scalar(@HIDObjects) > 0)
   {
      unlink($HIDLibrary);
      $BuildOutput = `iarchive --create @HIDObjects -o $HIDLibrary`;

      print $BuildOutput;

      $Folder = "generated_libs/profiles/";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/hid";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/hid/lib";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/hid/lib/ewarm";
      mkdir($Folder, 0700) unless (-d $Folder);

      $Folder = "generated_libs\\profiles\\hid\\lib\\ewarm\\";

      `move $HIDLibrary $Folder`;
   }

   #-------------------------------------------------------------------------
   # Create the MAP library with the specified files if requested
   #-------------------------------------------------------------------------
   if(scalar(@MAPObjects) > 0)
   {
      unlink($MAPLibrary);
      $BuildOutput = `iarchive --create @MAPObjects -o $MAPLibrary`;

      print $BuildOutput;

      $Folder = "generated_libs/profiles/";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/map";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/map/lib";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/map/lib/ewarm";
      mkdir($Folder, 0700) unless (-d $Folder);

      $Folder = "generated_libs\\profiles\\map\\lib\\ewarm\\";

      `move $MAPLibrary $Folder`;
   }

   #-------------------------------------------------------------------------
   # Create the PBAP library with the specified files if requested
   #-------------------------------------------------------------------------
   if(scalar(@PBAPObjects) > 0)
   {
      unlink($PBAPLibrary);
      $BuildOutput = `iarchive --create @PBAPObjects -o $PBAPLibrary`;

      print $BuildOutput;

      $Folder = "generated_libs/profiles/";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/pbap";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/pbap/lib";
      mkdir($Folder, 0700) unless (-d $Folder);
      $Folder = "generated_libs/profiles/pbap/lib/ewarm";
      mkdir($Folder, 0700) unless (-d $Folder);

      $Folder = "generated_libs\\profiles\\pbap\\lib\\ewarm\\";

      `move $PBAPLibrary $Folder`;
   }
}
