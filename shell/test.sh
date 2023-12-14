
# Linux创建指定大小文件

dd if=/dev/urandom of=10M.file bs=10MB count=1
dd if=/dev/zero of=1M.file bs=1MB count=1

