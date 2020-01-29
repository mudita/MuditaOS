#!/usr/bin/python2.7
# Get the Mobile Country Codes (MCC) and Mobile Network Codes (MNC) table
# from mcc-mnc.com and output it in JSON format.

import re
import urllib2
import json
import sqlite3 
td_re = re.compile('<td>([^<]*)</td>'*6)

html = urllib2.urlopen('http://mcc-mnc.com/').read()

tbody_start = False
conn = sqlite3.connect('country-codes.db')
c = conn.cursor()
c.execute("CREATE TABLE IF NOT EXISTS [codes] ([mcc] VARCHAR,[mnc] VARCHAR,[iso] VARCHAR,[country] VARCHAR,[country_code] VARCHAR,network VARCHAR)")
mcc_mnc_list = []

for line in html.split('\n'):
    if '<tbody>' in line:
        tbody_start = True
    elif '</tbody>' in line:
        break
    elif tbody_start:
        td_search = td_re.search(line)
        i = {}
        td_search = td_re.split(line)

        i['mcc'] = td_search[1]
        i['mnc'] = td_search[2]
        i['iso'] = td_search[3]
        i['country'] = td_search[4]
        i['country_code'] = td_search[5]
        i['network'] = td_search[6][0:-1]
        sql = ("INSERT INTO codes VALUES ('%s', '%s', '%s', \"%s\", '%s', \"%s\")" % (i['mcc'], i['mnc'], i['iso'], i['country'], i['country_code'], i['network']))
        print(sql)
        c.execute(sql)
        #, i['mnc'], i['iso'], i['country'], i['country_code'], i['network'])
conn.commit()
#print json.dumps(mcc_mnc_list, indent=2)
