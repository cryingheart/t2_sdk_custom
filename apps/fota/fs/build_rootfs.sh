ROOT_FS_DIR=../../../root_fs
TOOLS_DIR=../../../tools

#Take backup copy of root_fs/root and restore after image build
cp  -r $ROOT_FS_DIR/root $ROOT_FS_DIR/root_backup
cp   $ROOT_FS_DIR/root.img $ROOT_FS_DIR/root_backup.img

#Copy required files
cp ./part.json $ROOT_FS_DIR/root/part.json 
cp ./fota_config.json $ROOT_FS_DIR/root/fota_config.json 
curdir=$(pwd) 
echo $curdir

#build root.img
cd $ROOT_FS_DIR
./build.sh

cd $curdir

mv $ROOT_FS_DIR/root.img ../out/root.img

#Restore the package version of root
rm -rf $ROOT_FS_DIR/root
mv  $ROOT_FS_DIR/root_backup $ROOT_FS_DIR/root
mv  $ROOT_FS_DIR/root_backup.img $ROOT_FS_DIR/root.img
