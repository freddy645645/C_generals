#include "game.h"

#include <bits/stdc++.h>
using namespace std;
void onError(struct Header_Error_Res** res,size_t* reslen,int session, int errcode,const char * errmsg){
    *res=(Header_Error_Res*)calloc(1,HEADER_SIZE);
    *reslen=HEADER_SIZE;
    (**res).code=errcode;
    (**res).session=session;
    snprintf((**res).errmsg,sizeof((**res).errmsg),"%s",errmsg);
    return;
}
struct Node_Player{
    int session; 
    int room_id;
    int player_id;
    int player_state;
    int homeX,homeY;
    char name[16];
};
struct Node_Room{
    mutex room_mutex;
    vector<struct Node_Player> players;

    int game_state;
    int round;
    int room_id;
    vector<queue<struct Action>> action_queue;
    int player_number;
    int sizeX;
    int sizeY;
    vector<vector<struct Grid>> game_map;
    char passwd[16];

    void genMap();
    void next_round();
};
mutex list_mutex;
list<Node_Room*> Room_List;

int getSessin(){
    static int state=0;
    if(!state) state=rand();
    state=(state*20240101)+54321;
    return state;
}
Node_Room* getRoom(int room_id){
    list_mutex.lock();
    Node_Room* res=NULL;
    for(Node_Room* ptr:Room_List)
        if(ptr->room_id==room_id){
            res=ptr;
            break;
        }
    list_mutex.unlock();
    return res;
}
Node_Room* addRoom(int room_id){
    list_mutex.lock();
    Node_Room* room=new Node_Room;
    Room_List.push_back(room);
    room->room_id=room_id;
    list_mutex.unlock();
    return room;
}
void delRoom(int room_id){
    list_mutex.lock();
    for (std::list<Node_Room*>::iterator it = Room_List.begin(); it != Room_List.end(); it++) 
        if((*it)->room_id==room_id){
            delete *it;
            Room_List.erase(it);

            break;
        }
    
    list_mutex.unlock();
    return;

}
void setGameInfo(struct Header_Base** res,size_t* reslen, struct Node_Room* room,int player_id,int code){
    struct Node_Player* player=&(room->players[player_id]);
    size_t len=HEADER_SIZE+16*(max(0,room->player_number-2));
    struct Header_Room_Info_Res* header=(struct Header_Room_Info_Res*)calloc(1,len);
    header->code=code;
    header->session=player->session;
    header->room_id=player->room_id;
    header->player_id=player->player_id;
    header->player_number=room->player_number;
    header->sizeX=room->sizeX;
    header->sizeY=room->sizeY;
    header->game_state=room->game_state;
    for (int i=0;i<room->player_number;++i)
        memcpy(header->player_names[i],(room->players[i]).name,16);
    *res=(Header_Base*)header;
    *reslen=len;
    return;
        
}

bool verifySession(struct Header_Room_Info* header){
    struct Node_Room* room=getRoom(header->room_id);
    if(room==NULL) return false;
    if (header->player_id>=room->player_number||header->session!=room->players[header->player_id].session)
        return false;
    return true;
}

void Node_Room::genMap(){
    const double percent_mountain=0.1,percent_castle=0.05;
    vector<pair<int,int>> points;points.reserve(sizeX*sizeY);
    for(int i=0;i<sizeX;++i)for(int j=0;j<sizeY;++j){
        points.push_back({i,j});
    }
    auto rd = random_device {}; 
    auto rng = default_random_engine { rd() };
    shuffle(points.begin(), points.end(), rng);  
    game_map.assign(sizeX,vector<Grid>(sizeY));
    
    for(size_t i = 0;i<(size_t)player_number&&i<points.size();++i){
        auto [x,y]=points[i];
        game_map[x][y]=Grid(GAME_MAP_HOME,i,0);
        players[i].homeX=x;
        players[i].homeY=y;
    }
    uniform_real_distribution<double> uni(0,1);
    for(size_t i=player_number;i<points.size();++i){
        double res=uni(rng);
        auto [x,y]=points[i];
        if(res<percent_mountain){
            game_map[x][y]=Grid(GAME_MAP_MOUNTAIN,GAME_MAP_OWN_NEUTRAL,0);
        }else if(res<percent_mountain+percent_castle){
            game_map[x][y]=Grid(GAME_MAP_CASTLE,GAME_MAP_OWN_NEUTRAL,0);
        }else{
            game_map[x][y]=Grid(GAME_MAP_SPACE,GAME_MAP_OWN_NEUTRAL,0);
        }
    }

}
void delended(){
    
}
void Register_Room(struct Header_Base** res,size_t* reslen,struct Header_Room_Register* header){
    
    struct Node_Room* room=getRoom(header->room_id);
    if(room!=NULL){
        onError((Header_Error_Res**)res,reslen,0,RES_REGISTER_FAIL,"Room Already Exist");
        return;
    }
    room=addRoom(header->room_id);
    room->room_mutex.lock();
    room->player_number=header->player_number;
    room->players.resize(room->player_number);
    room->game_state=GAME_STATE_WAIT;
    room->round=0;
    room->room_id=header->room_id;

    room->sizeX=header->sizeX;
    room->sizeY=header->sizeY;
    room->game_map=vector<vector<struct Grid>>(room->sizeX,vector<struct Grid>(room->sizeY));
    room->genMap();
    memcpy(room->passwd,header->passwd,16);

    int player_id=-1;
    for(size_t i=0;i<room->players.size();++i){
        if(room->players[i].session==0){
            player_id=i;break;
        }
    }

    struct Node_Player* player=&room->players[player_id];
    player->session=getSessin();
    player->room_id=room->room_id;
    player->player_id=player_id;
    memcpy(player->name,header->name,16);
    player->player_state=PLAYER_STATE_ALIVE;
    

    printf("Room %d '%s' Added %d\n",header->room_id,player->name,player_id);

    setGameInfo(res,reslen,room,player->player_id,RES_REGISTER_SUCC);
    

    room->room_mutex.unlock();
    return;
    

}
void Join_Room(struct Header_Base** res ,size_t* reslen,struct Header_Room_Join* header){
    struct Node_Room* room=getRoom(header->room_id);
    if(room==NULL){
        onError((Header_Error_Res**)res,reslen, header->session, RES_JOIN_FAIL,"Room Doesn't Exist");
        return;
    }
    room->room_mutex.lock();

    int player_id=-1;
    for(size_t i=0;i<room->players.size();++i){
        if(room->players[i].session==0){
            player_id=i;break;
        }
    }

    if(player_id==-1){
        onError((Header_Error_Res**)res,reslen, header->session, RES_JOIN_FAIL,"Room Is Full");
        room->room_mutex.unlock();
        return;
    }
    
    if (strcmp(room->passwd,header->passwd)){
        onError((Header_Error_Res**)res,reslen, header->session, RES_JOIN_FAIL,"Password Is Wrong");
        room->room_mutex.unlock();
        return;
    }
    
    
    struct Node_Player* player=&(room->players[player_id]);
    player->session=getSessin();
    player->room_id=room->room_id;
    player->player_id=player_id;
    memcpy(player->name,header->name,16);
    player->player_state=PLAYER_STATE_ALIVE;

    printf("Room %d '%s' Added %d\n",header->room_id,player->name,player_id);
    

    setGameInfo(res,reslen,room,player->player_id,RES_JOIN_SUCC);
    room->room_mutex.unlock();
    return;
}
void Start_Game(struct Header_Base** res,size_t* reslen,struct Header_Start_Game* header){
    struct Node_Room* room=getRoom(header->room_id);
    if (verifySession((Header_Room_Info*)header)==false){
        onError((Header_Error_Res**)res,reslen,header->session, RES_START_GAME_FAIL,"Player Verify Error");
        return;
    }
    room->room_mutex.lock();
    if(room->game_state!=GAME_STATE_WAIT){
        onError((Header_Error_Res**)res,reslen,header->session, RES_START_GAME_FAIL,"Game Already Start");
        return;
    }

    struct Node_Player* player=&(room->players[header->player_id]);

    printf("Room %d Start by '%s'\n",header->room_id,player->name);
    room->game_state=GAME_STATE_INGAME;

    setGameInfo(res,reslen,room,player->player_id,RES_START_GAME_SUCC);
    room->room_mutex.unlock();
    return;
}
void Room_Info(Header_Base** res,size_t* reslen,Header_Room_Info* header ){
    struct Node_Room* room=getRoom(header->room_id);
    if (verifySession((Header_Room_Info*)header)==false){
        onError((Header_Error_Res**)res,reslen,header->session, RES_ROOM_INFO_FAIL,"Player Verify Error");
        return;
    }
    room->room_mutex.lock();
    struct Node_Player* player=&(room->players[header->player_id]);

    printf("Room %d '%s' Info Send\n",header->room_id,player->name);
    

    setGameInfo(res,reslen,room,player->player_id,RES_ROOM_INFO_SUCC);
    room->room_mutex.unlock();
    return;
}

void Quit_Game(Header_Base** res,size_t* reslen,Header_Quit* header){
    struct Node_Room* room=getRoom(header->room_id);
    if (verifySession((Header_Room_Info*)header)==false){
        onError((Header_Error_Res**)res,reslen,header->session, RES_QUIT_FAIL,"Player Verify Error");
        return;
    }
    room->room_mutex.lock();
    struct Node_Player* player=&(room->players[header->player_id]);
    if(room->game_state!=GAME_STATE_WAIT){
        player->player_state=PLAYER_STATE_QUITED;
    }else if(room->game_state==GAME_STATE_WAIT){
        *player=Node_Player();
    }
    
    printf("Room %d '%s' Quit\n",header->room_id,player->name);
    

    onError((Header_Error_Res**)res,reslen,header->session,RES_QUIT_SUCC,"Quit Success");
    room->room_mutex.unlock();
    return;    
}
void Add_Action(Header_Base** res,size_t* reslen,Header_Action* header){
    struct Node_Room* room=getRoom(header->room_id);
    if (verifySession((Header_Room_Info*)header)==false){
        onError((Header_Error_Res**)res,reslen,header->session, RES_ACTION_FAIL,"Player Verify Error");
        return;
    }
    room->room_mutex.lock();
    struct Node_Player* player=&(room->players[header->player_id]);
    if(room->game_state!=GAME_STATE_INGAME){
        onError((Header_Error_Res**)res,reslen,header->session, RES_ACTION_FAIL,"Game not Going");
        room->room_mutex.unlock();
        return;
    }
    if(player->player_state!=PLAYER_STATE_ALIVE){
        onError((Header_Error_Res**)res,reslen,header->session, RES_ACTION_FAIL,"You are Died/Quited");
        room->room_mutex.unlock();
        return;
    }

    for(int i=0;i<header->num_Action;++i){
        room->action_queue[player->player_id].push(header->action[i]);
    }
    
    
    printf("Room %d '%s' add %d action \n",header->room_id,player->name,header->num_Action);
    

    Header_Action_Res* tmp=(Header_Action_Res*)calloc(1,HEADER_SIZE);
    tmp->code=RES_ACTION_SUCC;
    tmp->session=header->session;
    tmp->room_id=header->room_id;
    tmp->player_id=header->player_id;
    tmp->num_Action=header->num_Action;
    *reslen=HEADER_SIZE;
    *res=(Header_Base*)tmp;
    room->room_mutex.unlock();
    return;    
}
void mapMask(Grid* masked_map,Node_Room* room,Node_Player* player){
    for(int i=0;i<room->sizeX;++i)
        for(int j=0;j<room->sizeY;++j){
            bool flag=false;
            for(int dx=-1;dx<=1;++dx)
                for(int dy=-1;dy<=1;++dy){
                    if(i+dx<0||i+dx>=room->sizeX||j+dy<0||j+dy>=room->sizeY) continue;
                    if(room->game_map[i+dx][j+dy].owner==player->player_id) flag=true;
                }
            if(flag||room->game_map[i][j].type==GAME_MAP_MOUNTAIN) 
                masked_map[i*room->sizeY+j]=room->game_map[i][j];
            else 
                masked_map[i*room->sizeY+j].type=GAME_MAP_FOG;
        }
    return;

}
void Map_Info(Header_Base** res,size_t* reslen,Header_Map_Info* header){
    struct Node_Room* room=getRoom(header->room_id);
    if (verifySession((Header_Room_Info*)header)==false){
        onError((Header_Error_Res**)res,reslen,header->session, RES_MAP_INFO_FAIL,"Player Verify Error");
        return;
    }
    room->room_mutex.lock();
    struct Node_Player* player=&(room->players[header->player_id]);
    if(room->game_state!=GAME_STATE_INGAME){
        onError((Header_Error_Res**)res,reslen,header->session, RES_MAP_INFO_FAIL,"Game not Going");
        room->room_mutex.unlock();
        return;
    }
    if(player->player_state!=PLAYER_STATE_ALIVE){
        onError((Header_Error_Res**)res,reslen,header->session, RES_MAP_INFO_FAIL,"You are Died/Quited");
        room->room_mutex.unlock();
        return;
    }
    printf("Room %d '%s' get Map Info\n",header->room_id,player->name);
    
    int lenNeed=HEADER_SIZE+sizeof(Grid)*(max(0,room->sizeX*room->sizeY-2));
    Header_Map_Info_Res* tmp=(Header_Map_Info_Res*)calloc(1,lenNeed);
    tmp->code=RES_MAP_INFO_SUCC;
    tmp->session=header->session;
    tmp->room_id=header->room_id;
    tmp->player_id=header->player_id;

    tmp->sizeX=room->sizeX;
    tmp->sizeY=room->sizeY;
    tmp->round=room->round;
    tmp->game_state=room->game_state;
    mapMask(tmp->grid,room,player);    

    *reslen=lenNeed;
    *res=(Header_Base*)tmp;
    
    room->room_mutex.unlock();
    return;    
}

void getStat(Player_Info* info,Node_Room* room,Node_Player* player){
    int n=room->player_number;
    memset(info,0,n*sizeof(Player_Info));
    for(int i=0;i<room->sizeX;++i)
        for(int j=0;j<room->sizeY;++j)
            if(room->game_map[i][j].owner>=0){
                info[room->game_map[i][j].owner].grid_num+=1;
                info[room->game_map[i][j].owner].soldier_num+=room->game_map[i][j].soldier_num;
            }
    for(int i=0;i<n;++i)
        info[i].player_state=room->players[i].player_state;
    return;
}

void Get_Player_Info(Header_Base** res,size_t* reslen,Header_Player_Info* header){
    struct Node_Room* room=getRoom(header->room_id);
    if (verifySession((Header_Room_Info*)header)==false){
        onError((Header_Error_Res**)res,reslen,header->session, RES_PLAYER_INFO_FAIL,"Player Verify Error");
        return;
    }
    room->room_mutex.lock();
    struct Node_Player* player=&(room->players[header->player_id]);
    if(room->game_state!=GAME_STATE_INGAME){
        onError((Header_Error_Res**)res,reslen,header->session, RES_PLAYER_INFO_FAIL,"Game not Going");
        room->room_mutex.unlock();
        return;
    }
    if(player->player_state!=PLAYER_STATE_ALIVE){
        onError((Header_Error_Res**)res,reslen,header->session, RES_PLAYER_INFO_FAIL,"You are Died/Quited");
        room->room_mutex.unlock();
        return;
    }
    
    
    printf("Room %d '%s' get User Info\n",header->room_id,player->name);
    
    int lenNeed=HEADER_SIZE+sizeof(Grid)*(max(0,room->player_number-2));
    Header_Player_Info_Res* tmp=(Header_Player_Info_Res*)calloc(1,lenNeed);
    tmp->code=RES_MAP_INFO_SUCC;
    tmp->session=header->session;
    tmp->room_id=header->room_id;
    tmp->player_id=header->player_id;
    tmp->game_state=room->game_state;
    //getStat(tmp->player,room,player);
    *reslen=lenNeed;
    *res=(Header_Base*)tmp;
    
    room->room_mutex.unlock();
    return;    
}

void Node_Room::next_round(){

    round=round+1;
    const int castle_round_need=1;
    const int space_round_need=25;
    for(int x=0;x<sizeX;++x)for(int y=0;y<sizeY;++y){

        if(round%castle_round_need==0 && game_map[x][y].type==GAME_MAP_HOME){
            game_map[x][y].soldier_num+=1;
        }
        if(round%castle_round_need==0 && game_map[x][y].type==GAME_MAP_CASTLE){
            game_map[x][y].soldier_num+=1;
        }
        if(round%space_round_need==0 && game_map[x][y].type==GAME_MAP_SPACE){
            if(game_map[x][y].owner!=GAME_MAP_OWN_NEUTRAL)
                game_map[x][y].soldier_num+=1;
        }
    }
    const int dx[]={1,0,-1,0};
    const int dy[]={0,1,0,-1};
    auto pos_check=[&](int x,int y){
        return x>=0&&x<sizeX&&y>=0&&y<sizeY;
    };
    for(int pid=0;pid<player_number;++pid){
        if(!action_queue[pid].empty()){
            Action act=action_queue[pid].front();
            action_queue[pid].pop();
            int cx=act.x;
            int cy=act.y;
            int nx=act.x+dx[act.way];
            int ny=act.y+dy[act.way];
            int half=act.all_or_half;
            if(!pos_check(cx,cy)||!pos_check(nx,ny)) continue;
            if(game_map[cx][cy].owner!=pid) continue;
            int soldier_trans=half?(game_map[cx][cy].soldier_num/2):(min(0,game_map[cx][cy].soldier_num-1));
            game_map[cx][cy].soldier_num-=soldier_trans;

            if(game_map[nx][ny].owner!=game_map[cx][cy].owner){
                if(soldier_trans>game_map[nx][ny].soldier_num){
                    game_map[nx][ny].owner=pid;
                    game_map[nx][ny].soldier_num=soldier_trans-game_map[nx][ny].soldier_num;                    
                }else{
                    game_map[nx][ny].soldier_num=game_map[nx][ny].soldier_num-soldier_trans;
                }
            }

        }
    }

    int player_alive=0;
    for(int pid=0;pid<player_number;++pid)
        if(game_map[players[pid].homeX][players[pid].homeY].owner!=pid)
            players[pid].player_state=PLAYER_STATE_DEAD;
        else if(players[pid].player_state==PLAYER_STATE_ALIVE)player_alive++;
    if(player_alive<=1) game_state=GAME_STATE_END;
}

void* GameHandler(void *arg){
    while(true){
        list_mutex.lock();
        for(auto room:Room_List){
            room->next_round();
        }
        list_mutex.unlock();
        delended();

        //const int microseconds=500*1000;
        sleep(1);
    }
   
}
