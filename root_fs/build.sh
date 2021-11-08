TOOLS_DIR=../tools

#Clean auto generated files

rm -f  root/part.checksum
rm -f  root/boot.checksum
rm -f  root/fota_config.checksum
rm -f ./root.img

#Copy flecher32 checksum generator
cp $TOOLS_DIR/fletcher32/fl32 ./root/fl32

#Create checksum files
echo "Creating checksum files..."
cd ./root
./fl32 part.json
./fl32 boot.json
./fl32 fota_config.json
rm -f fl32
cd ../
echo "Creating root image..."
#Create root image (root.img)
$TOOLS_DIR/mklittlefs/mklittlefs -s 0x40000 -c ./root ./root.img
