from pwn import *
r=remote('127.0.0.1',9880)

"""
res=r.recv(64+96)
print(len(res))

for i in range(0,len(res),4):
    print(res[i:i+4],u32(res[i:i+4],endian='big'))
"""

cmd_reg=p32(0x1,endian='big')+p32(0x0,endian='big')+p32(0x8763,endian='big')+p32(0x5,endian='big')+\
        p32(0x20,endian='big')+p32(0x20,endian='big')+b"I am first".ljust(16,b'\x00')+b"P@55w0rd".ljust(16,b'\x00')+b'\x00'*8
print(len(cmd_reg))
print(cmd_reg)
r.send(cmd_reg)


res=r.recv()
print(len(res))

for i in range(0,len(res),4):
    print(res[i:i+4],u32(res[i:i+4],endian='big'))


"""


"""

cmd_join=p32(0x2,endian='big')+p32(0x0,endian='big')+p32(0x8763,endian='big')+p32(0x5,endian='big')+\
        b"I am Second".ljust(16,b'\x00')+b"P@55w0rd".ljust(16,b'\x00')
print(len(cmd_join))
print(cmd_join)
cmd_join=cmd_join.ljust(64,b'\x00')
r.send(cmd_join)
        
res_join=r.recv()
print(len(res_join))

for i in range(0,len(res_join),4):
    print(res_join[i:i+4],u32(res_join[i:i+4],endian='big'))

    
raw_input()
cmd_info=p32(0x4,endian='big')+res_join[4:4+4*3]
cmd_info=cmd_info.ljust(64,b'\x00')
print(cmd_info)
r.send(cmd_info)

res_info=r.recv()
print(len(res_info))

for i in range(0,len(res_info),4):
    print(res_info[i:i+4],u32(res_info[i:i+4],endian='big'))


