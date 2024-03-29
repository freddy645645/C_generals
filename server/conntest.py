from pwn import *
import time
"""
res=r.recv(64+96)
print(len(res))

for i in range(0,len(res),4):
    print(res[i:i+4],u32(res[i:i+4],endian='big'))
"""
null=0
CMD_REGISTER   =0x1
CMD_JOIN       =0x2
CMD_START_GAME =0x3
CMD_ROOM_INFO  =0x4
CMD_ACTION     =0x5
CMD_MAP_INFO   =0x6
CMD_QUIT       =0x7
CMD_PLAYER_INFO=0x8
def parseErr(res):
    err={"code":u32(res[:4],endian='big',sign="signed"),
         "session":u32(res[4:8],endian='big',sign="signed"),
         "errmsg":res[32:]}
    return err
def parseInfo(res):
    code=u32(res[:4],endian='big',sign="signed")
    if((code//256)==0xF):
        return parseErr(res)
    info={}
    names=["code","session","room_id","player_id","player_number","szX","szY","game_state"]
    for i in range(len(names)):
        info[names[i]]=u32(res[4*i:4*i+4],endian='big',sign="signed")
    pname=[]
    for i in range(4*len(names),len(res),16):
        pname.append(res[i:i+16])
    info["pname"]=pname
    return info

def parseMap(res):
    code=u32(res[:4],endian='big',sign="signed")
    if((code//256)==0xF):
        return parseErr(res)
    info={}
    names=["code","session","room_id","player_id","szX","szY","round","game_state"]
    for i in range(len(names)):
        info[names[i]]=u32(res[4*i:4*i+4],endian='big',sign="signed")
    gmap=[[{} for i in range(info["szY"])]for i in range(info["szX"])]
    base=4*len(names)
    for i in range(info["szX"]):
        for j in range(info["szY"]):
            cur=(i*info["szY"]+j)
            grid=res[base+16*cur:base+16*cur+16];
            gname=["type","owner","snum"]
            for x in range(len(gname)):
                gmap[i][j][gname[x]]=u32(grid[4*x:4*x+4],endian='big',sign="signed")
                
    info["gmap"]=gmap
    return info
    
def prasePinfo(res,pnum):
    code=u32(res[:4],endian='big',sign="signed")
    if((code//256)==0xF):
        return parseErr(res)
    info={}
    names=["code","session","room_id","player_id","game_state"]
    for i in range(len(names)):
        info[names[i]]=u32(res[4*i:4*i+4],endian='big',sign="signed")
    
    
    base=8*4
    pinfo=[{} for x in range(base,len(res),16)]
    for st in range(base,len(res),16):
        pname=["grid_num", "soldier_num", "player_state"]
        ply=res[st:st+16]
        for x in range(len(pname)):
            pinfo[(st-base)//16][pname[x]]=u32(ply[4*x:4*x+4],endian='big',sign="signed")
    info["player"]=pinfo
        
    return info
    


def Send_Recv(r,msg):
    smsg= p32(u32(msg[:4],endian='big',sign="signed")+len(msg)*2**16,endian='big',sign="signed")+msg[4:]
    #print(smsg)
    r.send(smsg)

    res=r.recv(64)
    #print(res)
    Len=u32(res[:4],endian='big',sign="signed")//(2**16)
    rest=r.recv(Len-64) if Len>64 else b''
    
    res=p32(u32(res[:4],endian='big',sign="signed")%(2**16),endian='big',sign="signed")+res[4:]+rest
    return res

def Reg(r,rid,name,pwd,szX=8,szY=8,nplay=2):
    #cmd_reg=p32(64*(2**16)+0x1,endian='big')+p32(0x0,endian='big')+p32(0x8763,endian='big')+p32(0x5,endian='big')+\
    #        p32(0x20,endian='big')+p32(0x20,endian='big')+b'\x00'*8+b"I am first".ljust(16,b'\x00')+b"P@55w0rd".ljust(16,b'\x00')
    cmd_reg=flat([CMD_REGISTER,0,rid,nplay,szX,szY,null,null,name.encode().ljust(16,b'\x00'),pwd.encode().ljust(16,b'\x00')],endian='big',sign="signed")
    #print(cmd_reg)
    assert len(cmd_reg)==64
    res=Send_Recv(r,cmd_reg)
    #print(len(res))

    info=parseInfo(res)
    print(info)
    return info

def Join(r,rid,name,pwd):

    #cmd_join=p32(64*(2**16)+0x2,endian='big')+p32(0x0,endian='big')+p32(0x8763,endian='big')+p32(0x5,endian='big')+\
    #        b'\x00'*16+b"I am Second".ljust(16,b'\x00')+b"P@55w0rd".ljust(16,b'\x00')
    cmd_join=flat([CMD_JOIN,0,rid,[null]*5,name.encode().ljust(16,b'\x00'),pwd.encode().ljust(16,b'\x00')],endian='big',sign="signed")
    #print(len(cmd_join))
    assert len(cmd_join)==64
            
    res=Send_Recv(r,cmd_join)
    #print(len(res))

    info=parseInfo(res)
    print(info)
    return info

    
def RoomInfo(r,sess,rid,pid):
    #cmd_info=p32(64*(2**16)+0x4,endian='big')+res_join[4:4+4*3]
    #cmd_info=cmd_info.ljust(64,b'\x00')
    cmd_info=flat([CMD_ROOM_INFO,sess,rid,pid,[0]*12],endian='big',sign="signed")
    
    res=Send_Recv(r,cmd_info)
    
    info=parseInfo(res)
    print(info)
    return info

def StartGame(r,sess,rid,pid):
    cmd_start=flat([CMD_START_GAME,sess,rid,pid,[0]*12],endian='big',sign="signed")
    
    res=Send_Recv(r,cmd_start)
    
    info=parseInfo(res)
    print(info)
    return info
def QuitGame(r,sess,rid,pid):
    cmd_quit=flat([CMD_QUIT,sess,rid,pid,[0]*12],endian='big',sign="signed")
    
    res=Send_Recv(r,cmd_quit)
    
    info=parseErr(res)
    print(info)
    return info
    

def showMap(gmap):
    GAME_MAP_FOG        =-2
    GAME_MAP_MOUNTAIN   =-3
    GAME_MAP_CASTLE     =-4
    GAME_MAP_SPACE      =-5
    GAME_MAP_HOME       =-6
    typeslist=['~' for i in range(20)]
    typeslist[GAME_MAP_FOG]='X'
    typeslist[GAME_MAP_MOUNTAIN]='^'
    typeslist[GAME_MAP_CASTLE]='+'
    typeslist[GAME_MAP_SPACE]=' '
    typeslist[GAME_MAP_HOME]='*'
    from termcolor import HIGHLIGHTS,colored
    colorKeys=list(HIGHLIGHTS)[2:]
    for row in gmap:
        for grid in row:
            print(f'{"#" if grid["owner"]==-1 or grid["type"]==GAME_MAP_FOG else grid["owner"]}',end=' ')
        print('')
    ii=0
    for row in gmap:
        print(f"{ii:3}",end=' ')
        ii+=1
        for grid in row:
            type=typeslist[grid["type"]]
            snum=grid["snum"]
            print(colored(f"{type}{snum:3}",'grey',colorKeys[grid["owner"]]),end=' ')
        
        print()
def MapInfo(r,sess,rid,pid):
    cmd_map=flat([CMD_MAP_INFO,sess,rid,pid,[0]*12],endian='big',sign="signed")
    
    res=Send_Recv(r,cmd_map)
    
    info=parseMap(res)
    showMap(info["gmap"])
    return info

def Action(r,sess,rid,pid,acts):
    LL=len(acts)//4
    
    if len(acts)<8:
        acts+=[0]*(8-len(acts))
    cmd_act=flat([CMD_ACTION,sess,rid,pid,LL,[0]*3,acts],endian='big',sign="signed")
    res=Send_Recv(r,cmd_act)
    print(f"ACT OK:{u32(res[32:36],endian='big')}")
    return 
def PlayerInfo(r,sess,rid,pid):
    cmd_pinfo=flat([CMD_PLAYER_INFO,sess,rid,pid,[0]*12],endian='big',sign="signed")
    
    res=Send_Recv(r,cmd_pinfo)
    info=prasePinfo(res,2)
    print(info)
    return info

if __name__=="__main__":
    r=remote('localhost',9880)
    rid=int(input('Rid:'))
    nplay=int(input('nplay:'))
    passwd="passwd"
    players=[]
    p0=Reg(r,rid,input('username:'),passwd,nplay=nplay)
    players.append(p0)
    p2=Join(r,rid,'third',passwd)
    QuitGame(r,p2["session"],p2["room_id"],p2["player_id"])
    info0=RoomInfo(r,p0["session"],p0["room_id"],p0["player_id"])
    for i in range(nplay-1):
        pi=Join(r,rid,input('username:'),passwd)
        infoi=RoomInfo(r,p2["session"],p2["room_id"],p2["player_id"])
        players.append(pi)
    
    gs=StartGame(r,p0["session"],p0["room_id"],p0["player_id"])

    
    time.sleep(3)
    print('-------------')
    while True:
        MapInfo(r,998244353,p0["room_id"],-1)
        for player in players:
            MapInfo(r,player["session"],player["room_id"],player["player_id"])
            print('-------------')
        for player in players:
            acts=list(map(int,input(f'ACT P{player["player_id"]} {gs["pname"][player["player_id"]].decode()}:').split()))
            if len(acts): Action(r,player["session"],player["room_id"],player["player_id"],acts)
        
        PlayerInfo(r,p0["session"],p0["room_id"],p0["player_id"])
    r.close()