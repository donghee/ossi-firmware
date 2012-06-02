OSSIREADME 

OSSI-1 �ΰ����� �����

## BEACON ���� ##
1. BEACON�� 144.25MHZ ���� 12wpm ���� CW�� ��ȣ�� ��������. CW�� ���ڵ�� ���ڵ� �Ǿ� �ִ�
CW ��ȣ�� ������ 'OSSI/1' �� �ΰ����� �⺻������ ������.

	ù��°(6�� ����Ʈ) �ΰ����� �̸�
	
	    OSSI/1 
	
	�ι�°(5�� ����Ʈ) ����: 1 space errorflag space powerOn
	
		1 0b10100 0b11111 (��Ʈ����:EPS(BATTERY), OBC, BEACON, COMMS, Payload)
	
	����°(5�� ����Ʈ) ����: 2 space solarvoltage space batteryvoltage
	
		2 0b11111111 0b11111111
	
	�� ������ Space Space Space �� �����Ѵ�.

2. ��ȣ�б�
OSSI ����Ʈ�� ���� �ΰ������� �ڽ��� ����� �������� �����⸦ ������ 144.25MHZ�� ���߾� �ͷ� ������ �ȴ�.

3. �ΰ����� ã��
144.25MHZ���� CW��ȣ���� ������ OSSI/1�� ã�� ��´�. 


## COMMS�� ���� ##
���󿡼� 435.75MHZ�� 2FSK�� ���� ��Ŷ�� ������.

	from ossi import Uplink, Downlink, Request
	
	u = Uplink(435.75, '2FSK', 1200)
 	d = Downlink(435.75, '2FSK', 9600)
	request = Request(u, d)

	# EPS
	## �µ� 
	temp = request.getBatteryTemperature()
	temp = request.getPannelTemperature()
	x1_temp = request.getSolarPannelTemperature(X,1)
	z1_temp = request.getSolarPannelTemperature(Z,1)
	y1_temp = request.getSolarPannelTemperature(Y,1)

	## volatge
	v = request.getBatteryVoltage()
	v = request.getSolarPannelVoltage()

	## solar sensor
	s = request.getSolarSensor()

	## ���� ����
	check = request.checkBatteryCharge()
	

	## �ֶ�, ���͸�, SC ���� �ִ°�?
	check = request.checkSolarOn()
	check = request.checkBatteryOn()
	check = request.checkSuperCapOn()

	## ����ĸ 
	status = request.doSuperCapOn()
	check = request.checkSuperCapCharge()

	## EPS reset!
	request.resetEPS()

	# OBC
	## OBC reset!
	request.resetOBC()
	
	## ���̷�
	l = request.getGyro()

	## RTC �ð� �������� 
	time = request.getRtcTime()
	status = request.setRtcTime(datetime.datetime.now())

	## Payload LED
	reserveStatus = request.reserveMessage("OSSI 1", datetime.datetime(2012, 12, 12, 12, 12))
	reserveList = request.getReserveList()

	request.ledOn()
	request.ledOFF()
	
	## ���׳� ��������?
	check = request.checkAntDeploy()
	## ���׳� ���
	request.antDeploy()

	request.resetPayload()

	# COMMS
	request.resetCOMMS()

	# BEACON
	request.resetBEACON()

	# STORAGE(EEPROM, FRAM)
	status = request.setByteEEPROM(1, 'x')
	
http://swisscube-live.ch/Publish/S3-D-ICD-1-0a-Housekeeping_ICD.pdf



### ��Ŷ ���� ###

���� TX ��Ŷ

OxAA, 0xAA, 0xAA, 0xAA, 0xD3, 0x91, 0xAA, 0xAA, 0xAA, '1', '2', '3', '4', '5', '6', 0xAA, 0xAA, 0XAA

p1, p2, p3, p4, s1, s2, len, command1, c2, c3, d1~dn, crc

#### ax.25 ####
http://swisscube-live.ch/Publish/S3-BC-SE-1-1b-AX.25%20Transfer%20Frames%20Format.pdf

![](http://i.imgur.com/MygEt.png)

- 01111110 # flag
- callsign 48bits # destination
- ssid 8bit 011ssid0
- callsign 48bits # source
- ssid 8bit 011ssid0
- control bits 00000011
- protocal identifier 0xF0
- Information field(32-2048)
- Frame-Check Sequence
- 01111110 # flag

**Information field**

![](http://i.imgur.com/Xdnas.png)

- version number: 00
- virtual channel Id: 000
- spare: 000
- master frame count 0x00
- virtual channel frame count 0x00
- first header pointer 0x00
- data 
- time flag 0b000
- spare 00
- tc count 00
- time 

## UART�� ����ϱ� ##
OBC�� �ִ� �������̽��� ���ؼ� �ٸ� ����� ���� �� �� �ִ�. 

�������̽��� ���� 

	from ossi import Serial Request
	
 	s = Serial('Com3', 9600)
	request = Request(s)



## �ΰ����� dashboard ##

![](http://www.devco.net/images/gdash-small.png)

https://github.com/ripienaar/graphite-graph-dsl/wiki
