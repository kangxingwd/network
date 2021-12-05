




-netdev user,net=192.168.76.0/24,dhcpstart=192.168.76.9,hostfwd=tcp::5555-:5555,id=netdev0, -device e1000,netdev=netdev0,id=device-net0,mac=aa:54:00:11:22:33

