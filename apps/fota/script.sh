set -x

if [ "$#" -eq 0 ]
then
    dev="/dev/ttyUSB2"
else
    dev="$1"
fi

#Make the application
make -C ./

#Current working directory
cwd=$(pwd)

#build rootfs
cd fs
./build_rootfs.sh
cd $cwd
echo $cwd

#Get the environment Development(git) or SDK 
devel=`make disp_info | grep "DEVEL_PACKAGE" | head -n 1 | cut -d '=' -f 2`

if [ "$devel" -eq 1 ] ; then
    echo "Development environment"
    SDK_DIR=`make disp_info | grep "SDK_DIR" | head -n 1 | cut -d '=' -f 2`
    echo $SDK_DIR
else
    echo "SDK environment"
    SDK_DIR=../../
    echo $SDK_DIR
    
fi

#Rename fota.elf.strip to fota.elf
rm -f ./out/fota.elf
mv ./out/fota.elf.strip ./out/fota.elf

dd if=/dev/zero of=./empty.img bs=1K count=1

#erase
$SDK_DIR/script/boot.py  --device $dev --reset=evk42_bl $SDK_DIR/apps/gordon/bin/gordon.elf
$SDK_DIR/script/flash.py --device $dev write 0x1000 ./empty.img

$SDK_DIR/script/flash.py --device /dev/ttyUSB2 from_json ../../tools/partition_files/ssbl_part_table.json
# Download root fs image
$SDK_DIR/script/flash.py --device $dev write 0x1C0000 ./out/root.img

# Download ssbl
$SDK_DIR/script/flash.py --device $dev write 0x1000 ../ssbl/fast_ssbl.img

# Download atcmd Applicatin image (this is the .strip file renamed to .elf)
$SDK_DIR/script/flash.py --device $dev write 0x20000 ./out/fota.elf

# Reset the device
$SDK_DIR/script/boot.py --device $dev --reset=evk42
exit
