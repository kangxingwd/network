https://www.omnisecu.com/ccna-security/how-to-configure-site-to-site-ikev2-ipsec-vpn-using-pre-shared-key-authentication.php
https://blog.51cto.com/adamcrab/1959666

# 拓扑
# r1 0/0: 192.168.1.152     0/1 3.3.3.254
# r2 0/0: 192.168.1.153     0/1 2.2.2.254
# vnc1: 3.3.3.3
# vnc2: 2.2.2.2

# Step 1: Define IKEv2 Keyring
Router(config)#crypto ikev2 keyring kr1
Router(config-ikev2-keyring)#peer r2
Router(config-ikev2-keyring-peer)#address 192.168.1.153
Router(config-ikev2-keyring-peer)#pre-shared-key 123456
Router(config-ikev2-keyring-peer)#identity address 192.168.1.153
Router(config-ikev2-keyring-peer)#exit
Router(config-ikev2-keyring)#exit

# Step 2: Define IKEv2 Profiles
Router(config)#crypto ikev2 profile r1_profile
IKEv2 profile MUST have:
   1. A local and a remote authentication method.
   2. A match identity or a match certificate or match any statement.
Router(config-ikev2-profile)#authentication local pre-share 
Router(config-ikev2-profile)#authentication remote pre-share
Router(config-ikev2-profile)#dpd 30 5 periodic
Router(config-ikev2-profile)#match identity remote address 192.168.1.153
Router(config-ikev2-profile)#keyring local kr1
Router(config-ikev2-profile)#lifetime 3600
Router(config-ikev2-profile)#exit


# Step 3: Define IKEv2 Proposal
Router(config)#crypto ikev2 proposal r1_proposal
IKEv2 proposal MUST either have a set of an encryption algorithm other than aes-gcm, an integrity algorithm and a DH group configured or 
 encryption algorithm aes-gcm, a prf algorithm and a DH group configured                            
Router(config-ikev2-proposal)#encryption aes-cbc-256 
Router(config-ikev2-proposal)#integrity sha512 
Router(config-ikev2-proposal)#group 24
Router(config-ikev2-proposal)#prf sha256
Router(config-ikev2-proposal)#exit


# Step 4: Define IKEv2 Policies
Router(config)#crypto ikev2 policy r1_policy
IKEv2 policy MUST have atleast one complete proposal attached
Router(config-ikev2-policy)#proposal r1_proposal
Router(config-ikev2-policy)#exit


# Step 5: Define Crypto ACL to identify IPSec secured traffic
Router(config)#ip access-list extended r1_ip_acl
Router(config-ext-nacl)#permit ip 3.3.3.0 0.0.0.255 2.2.2.0 0.0.0.255
Router(config-ext-nacl)#exit


# Step 6: Define Transform Sets
Router(config)#crypto ipsec transform-set r1_ts esp-aes esp-sha512-hmac 
Router(cfg-crypto-trans)#mode tunnel
Router(cfg-crypto-trans)#exit


# Step 7: Define Crypto Maps
Router(config)#crypto map r1_map 10 ipsec-isakmp
% NOTE: This new crypto map will remain disabled until a peer
	and a valid access list have been configured.
Router(config-crypto-map)#set peer 192.168.1.153
Router(config-crypto-map)#set pfs group24
Router(config-crypto-map)#set security-association lifetime seconds 3600
Router(config-crypto-map)#set transform-set r1_ts
Router(config-crypto-map)#set ikev2-profile r1_profile
Router(config-crypto-map)#match address r1_ip_acl
Router(config-crypto-map)#exit


# Step 8: Activate Crypto Maps by applying the Crypto Map to Router's Interface
Router(config)#interface ethernet 0/0
Router(config-if)#crypto map r1_map
Router(config-if)#exit
Router(config)#
Dec  2 19:12:08.476: %CRYPTO-6-ISAKMP_ON_OFF: ISAKMP is ON
Router(config)#



# cisco 主动协商需要流量触发， 且需要将内网流量送到出接口，一般需要添加路由
ip route 2.2.2.0 255.255.255.0 192.168.1.153

