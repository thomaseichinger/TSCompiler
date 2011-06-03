% Trivial Script
% Thomas Eichinger
% May 30th 2011

# THIS IS A EARLY DRAFT
### Revision
* 0.1 		initial write down
* 0.1.1 	added explenation of APDU, adjusted data field in APDU and changed to 		
			CoffeeScript syntax

# Trivial Script

## Abstract

Trivial Script is an idea to provide scripting functionality to NXP P60 device.
It is intended to provide a fixed instruction set with fixed functionality means
opcode like one byte symbols which have a certain purposes. The purposes are hardcoded
routines using certain features of the P60 family

## Description

The main component would be a interpreter which maps an opcode to a purpose. Purposes should
be independend of each other, so the device remains responsive at all time.
The goal should be to be able to provide a interface to all/a subset of P60 features for 
scripting. Using this could simplify testing by providing the ability to write small test
sequences easily.

This should be especially useful if there is only few comunication between PCD and PICC.

### Workflow Overview/Example

+--------------------------------------------
| TrivialScript 0x01
|
| command_payload = ""
| 
| echo_payload = () ->
| 	command_payload = "Here comes data"
| 	command_payload += '0xaa 0xa8'
| 	send command_payload[ALL]
| 
|
| PROCESS = () -> echo_payload
+--------------------------------------------

					 ||
					 ||
					\  /
					 \/

	08 aa 01 00 55 ff 11 48 65 72 65 20 63 6f 6d 65 73 20 64 61 74 61 aa a8
		   |	 |  |  | --------------------------------------------------
		   |	 |  |  |						|
		   |	 |  |  |						|
		   |	 |  |  |						-- data field
		   |	 |  |  -- length of data field
		   |	 |  -- data field opcode
		   |	 -- opcode
		   -- TS version

					 ||
					 ||
					\  /
					 \/

+---------------------------------------------
|	...
|	case 0x55:	/* send */
| 		INS = SEND;
| 		if ( NEXT == 0xff )
|			send(PAYLOAD_BUFFER);
|	...
+---------------------------------------------

					 ||
					 ||
					\  /
					 \/

	48 65 72 65 20 63 6f 6d 65 73 20 64 61 74 61 aa a8 90 00
	
## Opcodes

t.b.d.

## TS SDK (a.k.a TODO)

* TS Interpreter
	- pack features into callable functions
	- TS parser
	- TS queue to be processed
* Documentation
	- covered features
	- list of opcodes
* JS/CoffeeScript/... to TS compiler for easy scripting