Talaria TWO SDK -- Release 2.3

Folder contents
===============

sdk_2.3
+--- apps      				        : Production ready example applications that run on Talaria TWO  
+--- binaries   				: Evaluation and Production applications that run on Talaria TWO    
	+--- eval 				: MPD binary for evaluation 
	+--- product 				: AT commands binary for production 
+--- components 				: Library components for functions 
+--- conf       			        : Config files for Make & Debugger 
+--- doc        				: User guides to setup SDK, reference manual etc.
+--- examples  				        : Example applications that run on Talaria TWO 
	+--- doc 				: Application note describing functionality
	+--- src 				: Source codes
	+--- bin 				: Firmware images - ELFs  
	+--- inc 				: Header files
+--- include   				        : SDK include files
+--- lib        				: SDK lib files
+--- pc_tools  				        : Programming and Multi-purpose demo tools
+--- script    				        : Helper scripts/utilities
+--- tools					: Tools, supporting files and scripts used for development on Talaria TWO
+--- root_fs					: Script to build the root_fs and basic files required for SSBL
+--- SDK_Release_Package_Quick_Start_Guide.docx : Introduction to the SDK release package
+--- SDK_Readme.txt				: This file
+--- build.mak				        : Makefile rules and variables for compilation
+--- .git_version 				: Git version of applications

Description of applications included:
apps/ble_provisioning 			        : Provisioning application using BLE as the mode of transferring provisioning data including example Android smartphone app
apps/fota				        : Basics of FOTA process 
apps/gordon         			        : Flash helper utility application
apps/helloworld     			        : Basic helloworld application
apps/iot_aws        		  	        : Using Talaria TWO board and the SDK with Amazon Web Services (AWS) IoT
apps/iot_azure      			        : Readme file with details on Connecting to Azure Cloud
apps/iperf3          			        : iperf3 server application on Talaria TWO
apps/ssbl				        : SSBL for enhancing flexibility of applications 
apps/stw            			        : Serial to Wi-Fi application over HIO
examples/at_custom_cmd			        : Using custom AT Commands
examples/ble_beacons         		        : Basics of Eddystone UID, URL and TLM Beacon applications
examples/ble_wifi_bridge      		        : Receiving a text message from a connected BLE client and publishing it to a CloudMQTT broker
examples/crash_handling	      		        : Using crash handler API to handle and debug error cases
examples/http_client	       		        : Using HTTP client APIs to connect to HTTP servers in secured and non-secured way
examples/innoos_memory_mgmt      	        : Basics of memory management and error handling
examples/innoos_msg_q	         	        : Using message queue APIs
examples/innoos_threads_semaphores	        : Using threads and semaphore APIs
examples/innoos_timers_callouts   	        : Using Timers and Callout APIs
examples/innoos_work_q            	        : Using Work Queue APIs
examples/lp_uart                 	        : Using UART APIs
examples/sleep_enable_disable		        : Basics of Sleep management in InnoOS™
examples/socket_wakeup			        : Basics of Sleep management in InnoOS™ - Talaria TWO wake-up from sleep mode
examples/spi_flash			        : Using SPI Flash APIs	
examples/un-assoc				: Using Wi-Fi un-associated mode transmission APIs
examples/using_ble             		        : Basics of BLE APIs consisting of a server and client application 
examples/using_wifi            		        : Using Wi-Fi connection manager APIs
examples/watchdog_timer        		        : Managing Talaria TWO watchdog timer 
examples/wcm_multi_ap				: Using Wi-Fi Connection with Multiple Access Point
examples/wcm_pm					: Basics of Wi-Fi Connection Manager Power Management APIs
examples/gpio					: Demo application describing the management of GPIO PINS of the Talaria TWO device
examples/adc					: Using the Analog to Digital Converter (ADC) peripheral of Talaria TWO modules
examples/pwm					: Using the Pulse Width Modulation (PWM) peripheral of Talaria TWO modules
examples/using_hio				: Basics of developing HIO based application on both host and Talaria TWO EVK






