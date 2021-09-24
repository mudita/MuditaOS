// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <array>
#include <string>

namespace utils::country
{
    enum class Id
    {
        AFGHANISTAN,
        ALAND_ISLANDS,
        ALBANIA,
        ALGERIA,
        AMERICAN_SAMOA,
        ANDORRA,
        ANGOLA,
        ANGUILLA,
        ANTARCTICA,
        ANTIGUA_AND_BARBUDA,
        ARGENTINA,
        ARMENIA,
        ARUBA,
        AUSTRALIA,
        AUSTRIA,
        AZERBAIJAN,
        BAHAMAS,
        BAHRAIN,
        BANGLADESH,
        BARBADOS,
        BELARUS,
        BELGIUM,
        BELIZE,
        BENIN,
        BERMUDA,
        BHUTAN,
        BOLIVIA,
        BOSNIA_AND_HERZEGOVINA,
        BOTSWANA,
        BOUVET_ISLAND,
        BRAZIL,
        BRITISH_VIRGIN_ISLANDS,
        BRITISH_INDIAN_OCEAN_TERRITORY,
        BRUNEI_DARUSSALAM,
        BULGARIA,
        BURKINA_FASO,
        BURUNDI,
        CAMBODIA,
        CAMEROON,
        CANADA,
        CAPE_VERDE,
        CAYMAN_ISLANDS,
        CENTRAL_AFRICAN_REPUBLIC,
        CHAD,
        CHILE,
        CHINA,
        HONG_KONG_SAR_CHINA,
        MACAO_SAR_CHINA,
        CHRISTMAS_ISLAND,
        COCOS_KEELING_ISLANDS,
        COLOMBIA,
        COMOROS,
        CONGO_BRAZZAVILLE,
        CONGO_KINSHASA,
        COOK_ISLANDS,
        COSTA_RICA,
        COTE_DIVOIRE,
        CROATIA,
        CUBA,
        CYPRUS,
        CZECH_REPUBLIC,
        DENMARK,
        DJIBOUTI,
        DOMINICA,
        DOMINICAN_REPUBLIC,
        ECUADOR,
        EGYPT,
        EL_SALVADOR,
        EQUATORIAL_GUINEA,
        ERITREA,
        ESTONIA,
        ETHIOPIA,
        FALKLAND_ISLANDS_MALVINAS,
        FAROE_ISLANDS,
        FIJI,
        FINLAND,
        FRANCE,
        FRENCH_GUIANA,
        FRENCH_POLYNESIA,
        FRENCH_SOUTHERN_TERRITORIES,
        GABON,
        GAMBIA,
        GEORGIA,
        GERMANY,
        GHANA,
        GIBRALTAR,
        GREECE,
        GREENLAND,
        GRENADA,
        GUADELOUPE,
        GUAM,
        GUATEMALA,
        GUERNSEY,
        GUINEA,
        GUINEABISSAU,
        GUYANA,
        HAITI,
        HEARD_AND_MCDONALD_ISLANDS,
        HOLY_SEE_VATICAN_CITY_STATE,
        HONDURAS,
        HUNGARY,
        ICELAND,
        INDIA,
        INDONESIA,
        IRAN_ISLAMIC_REPUBLIC_OF,
        IRAQ,
        IRELAND,
        ISLE_OF_MAN,
        ISRAEL,
        ITALY,
        JAMAICA,
        JAPAN,
        JERSEY,
        JORDAN,
        KAZAKHSTAN,
        KENYA,
        KIRIBATI,
        KOREA_NORTH,
        KOREA_SOUTH,
        KUWAIT,
        KYRGYZSTAN,
        LAO_PDR,
        LATVIA,
        LEBANON,
        LESOTHO,
        LIBERIA,
        LIBYA,
        LIECHTENSTEIN,
        LITHUANIA,
        LUXEMBOURG,
        MACEDONIA_REPUBLIC_OF,
        MADAGASCAR,
        MALAWI,
        MALAYSIA,
        MALDIVES,
        MALI,
        MALTA,
        MARSHALL_ISLANDS,
        MARTINIQUE,
        MAURITANIA,
        MAURITIUS,
        MAYOTTE,
        MEXICO,
        MICRONESIA_FEDERATED_STATES_OF,
        MOLDOVA,
        MONACO,
        MONGOLIA,
        MONTENEGRO,
        MONTSERRAT,
        MOROCCO,
        MOZAMBIQUE,
        MYANMAR,
        NAMIBIA,
        NAURU,
        NEPAL,
        NETHERLANDS,
        NETHERLANDS_ANTILLES,
        NEW_CALEDONIA,
        NEW_ZEALAND,
        NICARAGUA,
        NIGER,
        NIGERIA,
        NIUE,
        NORFOLK_ISLAND,
        NORTHERN_MARIANA_ISLANDS,
        NORWAY,
        OMAN,
        PAKISTAN,
        PALAU,
        PALESTINIAN_TERRITORY,
        PANAMA,
        PAPUA_NEW_GUINEA,
        PARAGUAY,
        PERU,
        PHILIPPINES,
        PITCAIRN,
        POLAND,
        PORTUGAL,
        PUERTO_RICO,
        QATAR,
        REUNION,
        ROMANIA,
        RUSSIAN_FEDERATION,
        RWANDA,
        SAINTBARTHELEMY,
        SAINT_HELENA,
        SAINT_KITTS_AND_NEVIS,
        SAINT_LUCIA,
        SAINTMARTIN_FRENCH_PART,
        SAINT_PIERRE_AND_MIQUELON,
        SAINT_VINCENT_AND_GRENADINES,
        SAMOA,
        SAN_MARINO,
        SAO_TOME_AND_PRINCIPE,
        SAUDI_ARABIA,
        SENEGAL,
        SERBIA,
        SEYCHELLES,
        SIERRA_LEONE,
        SINGAPORE,
        SLOVAKIA,
        SLOVENIA,
        SOLOMON_ISLANDS,
        SOMALIA,
        SOUTH_AFRICA,
        SOUTH_GEORGIA_AND_THE_SOUTH_SANDWICH_ISLANDS,
        SOUTH_SUDAN,
        SPAIN,
        SRI_LANKA,
        SUDAN,
        SURINAME,
        SVALBARD_AND_JAN_MAYEN_ISLANDS,
        SWAZILAND,
        SWEDEN,
        SWITZERLAND,
        SYRIAN_ARAB_REPUBLIC_SYRIA,
        TAIWAN_REPUBLIC_OF_CHINA,
        TAJIKISTAN,
        TANZANIA_UNITED_REPUBLIC_OF,
        THAILAND,
        TIMORLESTE,
        TOGO,
        TOKELAU,
        TONGA,
        TRINIDAD_AND_TOBAGO,
        TUNISIA,
        TURKEY,
        TURKMENISTAN,
        TURKS_AND_CAICOS_ISLANDS,
        TUVALU,
        UGANDA,
        UKRAINE,
        UNITED_ARAB_EMIRATES,
        UNITED_KINGDOM,
        UNITED_STATES_OF_AMERICA,
        US_MINOR_OUTLYING_ISLANDS,
        URUGUAY,
        UZBEKISTAN,
        VANUATU,
        VENEZUELA_BOLIVARIAN_REPUBLIC,
        VIET_NAM,
        VIRGIN_ISLANDS_US,
        WALLIS_AND_FUTUNA_ISLANDS,
        WESTERN_SAHARA,
        YEMEN,
        ZAMBIA,
        ZIMBABWE,

        // keep it last
        UNKNOWN,
    };

    inline constexpr auto count          = static_cast<std::size_t>(Id::UNKNOWN) + 1;
    inline constexpr auto defaultCountry = Id::UNKNOWN;

    struct Data
    {
        const char *name;
        const char *alpha2;
        const char *alpha3;
        unsigned int un_number;
    };

    inline constexpr std::array<struct Data, count> map = {{
        {"Afghanistan", "AF", "AFG", 4},                       // AFGHANISTAN
        {"ALA Aland Islands", "AX", "ALA", 248},               // ALA_ALAND_ISLANDS
        {"Albania", "AL", "ALB", 8},                           // ALBANIA
        {"Algeria", "DZ", "DZA", 12},                          // ALGERIA
        {"American Samoa", "AS", "ASM", 16},                   // AMERICAN_SAMOA
        {"Andorra", "AD", "AND", 20},                          // ANDORRA
        {"Angola", "AO", "AGO", 24},                           // ANGOLA
        {"Anguilla", "AI", "AIA", 660},                        // ANGUILLA
        {"Antarctica", "AQ", "ATA", 10},                       // ANTARCTICA
        {"Antigua and Barbuda", "AG", "ATG", 28},              // ANTIGUA_AND_BARBUDA
        {"Argentina", "AR", "ARG", 32},                        // ARGENTINA
        {"Armenia", "AM", "ARM", 51},                          // ARMENIA
        {"Aruba", "AW", "ABW", 533},                           // ARUBA
        {"Australia", "AU", "AUS", 36},                        // AUSTRALIA
        {"Austria", "AT", "AUT", 40},                          // AUSTRIA
        {"Azerbaijan", "AZ", "AZE", 31},                       // AZERBAIJAN
        {"Bahamas", "BS", "BHS", 44},                          // BAHAMAS
        {"Bahrain", "BH", "BHR", 48},                          // BAHRAIN
        {"Bangladesh", "BD", "BGD", 50},                       // BANGLADESH
        {"Barbados", "BB", "BRB", 52},                         // BARBADOS
        {"Belarus", "BY", "BLR", 112},                         // BELARUS
        {"Belgium", "BE", "BEL", 56},                          // BELGIUM
        {"Belize", "BZ", "BLZ", 84},                           // BELIZE
        {"Benin", "BJ", "BEN", 204},                           // BENIN
        {"Bermuda", "BM", "BMU", 60},                          // BERMUDA
        {"Bhutan", "BT", "BTN", 64},                           // BHUTAN
        {"Bolivia", "BO", "BOL", 68},                          // BOLIVIA
        {"Bosnia and Herzegovina", "BA", "BIH", 70},           // BOSNIA_AND_HERZEGOVINA
        {"Botswana", "BW", "BWA", 72},                         // BOTSWANA
        {"Bouvet Island", "BV", "BVT", 74},                    // BOUVET_ISLAND
        {"Brazil", "BR", "BRA", 76},                           // BRAZIL
        {"British Virgin Islands", "VG", "VGB", 92},           // BRITISH_VIRGIN_ISLANDS
        {"British Indian Ocean Territory", "IO", "IOT", 86},   // BRITISH_INDIAN_OCEAN_TERRITORY
        {"Brunei Darussalam", "BN", "BRN", 96},                // BRUNEI_DARUSSALAM
        {"Bulgaria", "BG", "BGR", 100},                        // BULGARIA
        {"Burkina Faso", "BF", "BFA", 854},                    // BURKINA_FASO
        {"Burundi", "BI", "BDI", 108},                         // BURUNDI
        {"Cambodia", "KH", "KHM", 116},                        // CAMBODIA
        {"Cameroon", "CM", "CMR", 120},                        // CAMEROON
        {"Canada", "CA", "CAN", 124},                          // CANADA
        {"Cape Verde", "CV", "CPV", 132},                      // CAPE_VERDE
        {"Cayman Islands", "KY", "CYM", 136},                  // CAYMAN_ISLANDS
        {"Central African Republic", "CF", "CAF", 140},        // CENTRAL_AFRICAN_REPUBLIC
        {"Chad", "TD", "TCD", 148},                            // CHAD
        {"Chile", "CL", "CHL", 152},                           // CHILE
        {"China", "CN", "CHN", 156},                           // CHINA
        {"Hong Kong, SAR China", "HK", "HKG", 344},            // HONG_KONG_SAR_CHINA
        {"Macao, SAR China", "MO", "MAC", 446},                // MACAO_SAR_CHINA
        {"Christmas Island", "CX", "CXR", 162},                // CHRISTMAS_ISLAND
        {"Cocos (Keeling) Islands", "CC", "CCK", 166},         // COCOS_KEELING_ISLANDS
        {"Colombia", "CO", "COL", 170},                        // COLOMBIA
        {"Comoros", "KM", "COM", 174},                         // COMOROS
        {"Congo (Brazzaville)", "CG", "COG", 178},             // CONGO_BRAZZAVILLE
        {"Congo, (Kinshasa)", "CD", "COD", 180},               // CONGO_KINSHASA
        {"Cook Islands", "CK", "COK", 184},                    // COOK_ISLANDS
        {"Costa Rica", "CR", "CRI", 188},                      // COSTA_RICA
        {"Côte d'Ivoire", "CI", "CIV", 384},                   // CÔTE_DIVOIRE
        {"Croatia", "HR", "HRV", 191},                         // CROATIA
        {"Cuba", "CU", "CUB", 192},                            // CUBA
        {"Cyprus", "CY", "CYP", 196},                          // CYPRUS
        {"Czech Republic", "CZ", "CZE", 203},                  // CZECH_REPUBLIC
        {"Denmark", "DK", "DNK", 208},                         // DENMARK
        {"Djibouti", "DJ", "DJI", 262},                        // DJIBOUTI
        {"Dominica", "DM", "DMA", 212},                        // DOMINICA
        {"Dominican Republic", "DO", "DOM", 214},              // DOMINICAN_REPUBLIC
        {"Ecuador", "EC", "ECU", 218},                         // ECUADOR
        {"Egypt", "EG", "EGY", 818},                           // EGYPT
        {"El Salvador", "SV", "SLV", 222},                     // EL_SALVADOR
        {"Equatorial Guinea", "GQ", "GNQ", 226},               // EQUATORIAL_GUINEA
        {"Eritrea", "ER", "ERI", 232},                         // ERITREA
        {"Estonia", "EE", "EST", 233},                         // ESTONIA
        {"Ethiopia", "ET", "ETH", 231},                        // ETHIOPIA
        {"Falkland Islands (Malvinas)", "FK", "FLK", 238},     // FALKLAND_ISLANDS_MALVINAS
        {"Faroe Islands", "FO", "FRO", 234},                   // FAROE_ISLANDS
        {"Fiji", "FJ", "FJI", 242},                            // FIJI
        {"Finland", "FI", "FIN", 246},                         // FINLAND
        {"France", "FR", "FRA", 250},                          // FRANCE
        {"French Guiana", "GF", "GUF", 254},                   // FRENCH_GUIANA
        {"French Polynesia", "PF", "PYF", 258},                // FRENCH_POLYNESIA
        {"French Southern Territories", "TF", "ATF", 260},     // FRENCH_SOUTHERN_TERRITORIES
        {"Gabon", "GA", "GAB", 266},                           // GABON
        {"Gambia", "GM", "GMB", 270},                          // GAMBIA
        {"Georgia", "GE", "GEO", 268},                         // GEORGIA
        {"Germany", "DE", "DEU", 276},                         // GERMANY
        {"Ghana", "GH", "GHA", 288},                           // GHANA
        {"Gibraltar", "GI", "GIB", 292},                       // GIBRALTAR
        {"Greece", "GR", "GRC", 300},                          // GREECE
        {"Greenland", "GL", "GRL", 304},                       // GREENLAND
        {"Grenada", "GD", "GRD", 308},                         // GRENADA
        {"Guadeloupe", "GP", "GLP", 312},                      // GUADELOUPE
        {"Guam", "GU", "GUM", 316},                            // GUAM
        {"Guatemala", "GT", "GTM", 320},                       // GUATEMALA
        {"Guernsey", "GG", "GGY", 831},                        // GUERNSEY
        {"Guinea", "GN", "GIN", 324},                          // GUINEA
        {"Guinea-Bissau", "GW", "GNB", 624},                   // GUINEABISSAU
        {"Guyana", "GY", "GUY", 328},                          // GUYANA
        {"Haiti", "HT", "HTI", 332},                           // HAITI
        {"Heard and Mcdonald Islands", "HM", "HMD", 334},      // HEARD_AND_MCDONALD_ISLANDS
        {"Holy See (Vatican City State)", "VA", "VAT", 336},   // HOLY_SEE_VATICAN_CITY_STATE
        {"Honduras", "HN", "HND", 340},                        // HONDURAS
        {"Hungary", "HU", "HUN", 348},                         // HUNGARY
        {"Iceland", "IS", "ISL", 352},                         // ICELAND
        {"India", "IN", "IND", 356},                           // INDIA
        {"Indonesia", "ID", "IDN", 360},                       // INDONESIA
        {"Iran, Islamic Republic of", "IR", "IRN", 364},       // IRAN_ISLAMIC_REPUBLIC_OF
        {"Iraq", "IQ", "IRQ", 368},                            // IRAQ
        {"Ireland", "IE", "IRL", 372},                         // IRELAND
        {"Isle of Man", "IM", "IMN", 833},                     // ISLE_OF_MAN
        {"Israel", "IL", "ISR", 376},                          // ISRAEL
        {"Italy", "IT", "ITA", 380},                           // ITALY
        {"Jamaica", "JM", "JAM", 388},                         // JAMAICA
        {"Japan", "JP", "JPN", 392},                           // JAPAN
        {"Jersey", "JE", "JEY", 832},                          // JERSEY
        {"Jordan", "JO", "JOR", 400},                          // JORDAN
        {"Kazakhstan", "KZ", "KAZ", 398},                      // KAZAKHSTAN
        {"Kenya", "KE", "KEN", 404},                           // KENYA
        {"Kiribati", "KI", "KIR", 296},                        // KIRIBATI
        {"Korea (North)", "KP", "PRK", 408},                   // KOREA_NORTH
        {"Korea (South)", "KR", "KOR", 410},                   // KOREA_SOUTH
        {"Kuwait", "KW", "KWT", 414},                          // KUWAIT
        {"Kyrgyzstan", "KG", "KGZ", 417},                      // KYRGYZSTAN
        {"Lao PDR", "LA", "LAO", 418},                         // LAO_PDR
        {"Latvia", "LV", "LVA", 428},                          // LATVIA
        {"Lebanon", "LB", "LBN", 422},                         // LEBANON
        {"Lesotho", "LS", "LSO", 426},                         // LESOTHO
        {"Liberia", "LR", "LBR", 430},                         // LIBERIA
        {"Libya", "LY", "LBY", 434},                           // LIBYA
        {"Liechtenstein", "LI", "LIE", 438},                   // LIECHTENSTEIN
        {"Lithuania", "LT", "LTU", 440},                       // LITHUANIA
        {"Luxembourg", "LU", "LUX", 442},                      // LUXEMBOURG
        {"Macedonia, Republic of", "MK", "MKD", 807},          // MACEDONIA_REPUBLIC_OF
        {"Madagascar", "MG", "MDG", 450},                      // MADAGASCAR
        {"Malawi", "MW", "MWI", 454},                          // MALAWI
        {"Malaysia", "MY", "MYS", 458},                        // MALAYSIA
        {"Maldives", "MV", "MDV", 462},                        // MALDIVES
        {"Mali", "ML", "MLI", 466},                            // MALI
        {"Malta", "MT", "MLT", 470},                           // MALTA
        {"Marshall Islands", "MH", "MHL", 584},                // MARSHALL_ISLANDS
        {"Martinique", "MQ", "MTQ", 474},                      // MARTINIQUE
        {"Mauritania", "MR", "MRT", 478},                      // MAURITANIA
        {"Mauritius", "MU", "MUS", 480},                       // MAURITIUS
        {"Mayotte", "YT", "MYT", 175},                         // MAYOTTE
        {"Mexico", "MX", "MEX", 484},                          // MEXICO
        {"Micronesia, Federated States of", "FM", "FSM", 583}, // MICRONESIA_FEDERATED_STATES_OF
        {"Moldova", "MD", "MDA", 498},                         // MOLDOVA
        {"Monaco", "MC", "MCO", 492},                          // MONACO
        {"Mongolia", "MN", "MNG", 496},                        // MONGOLIA
        {"Montenegro", "ME", "MNE", 499},                      // MONTENEGRO
        {"Montserrat", "MS", "MSR", 500},                      // MONTSERRAT
        {"Morocco", "MA", "MAR", 504},                         // MOROCCO
        {"Mozambique", "MZ", "MOZ", 508},                      // MOZAMBIQUE
        {"Myanmar", "MM", "MMR", 104},                         // MYANMAR
        {"Namibia", "NA", "NAM", 516},                         // NAMIBIA
        {"Nauru", "NR", "NRU", 520},                           // NAURU
        {"Nepal", "NP", "NPL", 524},                           // NEPAL
        {"Netherlands", "NL", "NLD", 528},                     // NETHERLANDS
        {"Netherlands Antilles", "AN", "ANT", 530},            // NETHERLANDS_ANTILLES
        {"New Caledonia", "NC", "NCL", 540},                   // NEW_CALEDONIA
        {"New Zealand", "NZ", "NZL", 554},                     // NEW_ZEALAND
        {"Nicaragua", "NI", "NIC", 558},                       // NICARAGUA
        {"Niger", "NE", "NER", 562},                           // NIGER
        {"Nigeria", "NG", "NGA", 566},                         // NIGERIA
        {"Niue", "NU", "NIU", 570},                            // NIUE
        {"Norfolk Island", "NF", "NFK", 574},                  // NORFOLK_ISLAND
        {"Northern Mariana Islands", "MP", "MNP", 580},        // NORTHERN_MARIANA_ISLANDS
        {"Norway", "NO", "NOR", 578},                          // NORWAY
        {"Oman", "OM", "OMN", 512},                            // OMAN
        {"Pakistan", "PK", "PAK", 586},                        // PAKISTAN
        {"Palau", "PW", "PLW", 585},                           // PALAU
        {"Palestinian Territory", "PS", "PSE", 275},           // PALESTINIAN_TERRITORY
        {"Panama", "PA", "PAN", 591},                          // PANAMA
        {"Papua New Guinea", "PG", "PNG", 598},                // PAPUA_NEW_GUINEA
        {"Paraguay", "PY", "PRY", 600},                        // PARAGUAY
        {"Peru", "PE", "PER", 604},                            // PERU
        {"Philippines", "PH", "PHL", 608},                     // PHILIPPINES
        {"Pitcairn", "PN", "PCN", 612},                        // PITCAIRN
        {"Poland", "PL", "POL", 616},                          // POLAND
        {"Portugal", "PT", "PRT", 620},                        // PORTUGAL
        {"Puerto Rico", "PR", "PRI", 630},                     // PUERTO_RICO
        {"Qatar", "QA", "QAT", 634},                           // QATAR
        {"Réunion", "RE", "REU", 638},                         // RÉUNION
        {"Romania", "RO", "ROU", 642},                         // ROMANIA
        {"Russian Federation", "RU", "RUS", 643},              // RUSSIAN_FEDERATION
        {"Rwanda", "RW", "RWA", 646},                          // RWANDA
        {"Saint-Barthélemy", "BL", "BLM", 652},                // SAINTBARTHÉLEMY
        {"Saint Helena", "SH", "SHN", 654},                    // SAINT_HELENA
        {"Saint Kitts and Nevis", "KN", "KNA", 659},           // SAINT_KITTS_AND_NEVIS
        {"Saint Lucia", "LC", "LCA", 662},                     // SAINT_LUCIA
        {"Saint-Martin (French part)", "MF", "MAF", 663},      // SAINTMARTIN_FRENCH_PART
        {"Saint Pierre and Miquelon", "PM", "SPM", 666},       // SAINT_PIERRE_AND_MIQUELON
        {"Saint Vincent and Grenadines", "VC", "VCT", 670},    // SAINT_VINCENT_AND_GRENADINES
        {"Samoa", "WS", "WSM", 882},                           // SAMOA
        {"San Marino", "SM", "SMR", 674},                      // SAN_MARINO
        {"Sao Tome and Principe", "ST", "STP", 678},           // SAO_TOME_AND_PRINCIPE
        {"Saudi Arabia", "SA", "SAU", 682},                    // SAUDI_ARABIA
        {"Senegal", "SN", "SEN", 686},                         // SENEGAL
        {"Serbia", "RS", "SRB", 688},                          // SERBIA
        {"Seychelles", "SC", "SYC", 690},                      // SEYCHELLES
        {"Sierra Leone", "SL", "SLE", 694},                    // SIERRA_LEONE
        {"Singapore", "SG", "SGP", 702},                       // SINGAPORE
        {"Slovakia", "SK", "SVK", 703},                        // SLOVAKIA
        {"Slovenia", "SI", "SVN", 705},                        // SLOVENIA
        {"Solomon Islands", "SB", "SLB", 90},                  // SOLOMON_ISLANDS
        {"Somalia", "SO", "SOM", 706},                         // SOMALIA
        {"South Africa", "ZA", "ZAF", 710},                    // SOUTH_AFRICA
        {"South Georgia and the South Sandwich Islands",
         "GS",
         "SGS",
         239},                                                 // SOUTH_GEORGIA_AND_THE_SOUTH_SANDWICH_ISLANDS
        {"South Sudan", "SS", "SSD", 728},                     // SOUTH_SUDAN
        {"Spain", "ES", "ESP", 724},                           // SPAIN
        {"Sri Lanka", "LK", "LKA", 144},                       // SRI_LANKA
        {"Sudan", "SD", "SDN", 736},                           // SUDAN
        {"Suriname", "SR", "SUR", 740},                        // SURINAME
        {"Svalbard and Jan Mayen Islands", "SJ", "SJM", 744},  // SVALBARD_AND_JAN_MAYEN_ISLANDS
        {"Swaziland", "SZ", "SWZ", 748},                       // SWAZILAND
        {"Sweden", "SE", "SWE", 752},                          // SWEDEN
        {"Switzerland", "CH", "CHE", 756},                     // SWITZERLAND
        {"Syrian Arab Republic (Syria)", "SY", "SYR", 760},    // SYRIAN_ARAB_REPUBLIC_SYRIA
        {"Taiwan, Republic of China", "TW", "TWN", 158},       // TAIWAN_REPUBLIC_OF_CHINA
        {"Tajikistan", "TJ", "TJK", 762},                      // TAJIKISTAN
        {"Tanzania, United Republic of", "TZ", "TZA", 834},    // TANZANIA_UNITED_REPUBLIC_OF
        {"Thailand", "TH", "THA", 764},                        // THAILAND
        {"Timor-Leste", "TL", "TLS", 626},                     // TIMORLESTE
        {"Togo", "TG", "TGO", 768},                            // TOGO
        {"Tokelau", "TK", "TKL", 772},                         // TOKELAU
        {"Tonga", "TO", "TON", 776},                           // TONGA
        {"Trinidad and Tobago", "TT", "TTO", 780},             // TRINIDAD_AND_TOBAGO
        {"Tunisia", "TN", "TUN", 788},                         // TUNISIA
        {"Turkey", "TR", "TUR", 792},                          // TURKEY
        {"Turkmenistan", "TM", "TKM", 795},                    // TURKMENISTAN
        {"Turks and Caicos Islands", "TC", "TCA", 796},        // TURKS_AND_CAICOS_ISLANDS
        {"Tuvalu", "TV", "TUV", 798},                          // TUVALU
        {"Uganda", "UG", "UGA", 800},                          // UGANDA
        {"Ukraine", "UA", "UKR", 804},                         // UKRAINE
        {"United Arab Emirates", "AE", "ARE", 784},            // UNITED_ARAB_EMIRATES
        {"United Kingdom", "GB", "GBR", 826},                  // UNITED_KINGDOM
        {"United States of America", "US", "USA", 840},        // UNITED_STATES_OF_AMERICA
        {"US Minor Outlying Islands", "UM", "UMI", 581},       // US_MINOR_OUTLYING_ISLANDS
        {"Uruguay", "UY", "URY", 858},                         // URUGUAY
        {"Uzbekistan", "UZ", "UZB", 860},                      // UZBEKISTAN
        {"Vanuatu", "VU", "VUT", 548},                         // VANUATU
        {"Venezuela (Bolivarian Republic)", "VE", "VEN", 862}, // VENEZUELA_BOLIVARIAN_REPUBLIC
        {"Viet Nam", "VN", "VNM", 704},                        // VIET_NAM
        {"Virgin Islands, US", "VI", "VIR", 850},              // VIRGIN_ISLANDS_US
        {"Wallis and Futuna Islands", "WF", "WLF", 876},       // WALLIS_AND_FUTUNA_ISLANDS
        {"Western Sahara", "EH", "ESH", 732},                  // WESTERN_SAHARA
        {"Yemen", "YE", "YEM", 887},                           // YEMEN
        {"Zambia", "ZM", "ZMB", 894},                          // ZAMBIA
        {"Zimbabwe", "ZW", "ZWE", 716},                        // ZIMBABWE
        {"Unknown", "ZZ", "ZZZ", 0},                           // UNKNOWN
    }};

    constexpr const char *getName(Id id)
    {
        return map[static_cast<std::size_t>(id)].name;
    }

    constexpr const char *getAlpha2Code(Id id)
    {
        return map[static_cast<std::size_t>(id)].alpha2;
    }

    constexpr const char *getAlpha3Code(Id id)
    {
        return map[static_cast<std::size_t>(id)].alpha3;
    }

    constexpr unsigned int getUNnumber(Id id)
    {
        return map[static_cast<std::size_t>(id)].un_number;
    }

    /**
     * @brief Get the country Id By its Alpha2 code value, e.g. "PL" marches Id::POLAND
     *
     * @param alpha2Code
     * @return Id
     */
    inline Id getIdByAlpha2Code(const std::string &alpha2Code)
    {
        for (std::size_t i = 0; i < count; i++) {
            if (std::string(map[i].alpha2) == alpha2Code) {
                return static_cast<Id>(i);
            }
        }

        return Id::UNKNOWN;
    }

} // namespace utils::country
