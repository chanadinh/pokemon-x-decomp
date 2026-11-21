#include "battle_moves_b4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, BUF=0x7800, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Values returned by the external services. */
static uint32_t lm_value,hs_value,eb_value,hm_value,tf_value,tr_value,cs_value,mb_value,slot_new,slot_own,cost_value,room_value;
static uint32_t pv[8][16],rs[8][16],has_mask,rnd_value,count_value;static uint8_t list_values[16];
static unsigned aborts,ncommit;static uint8_t commits[12][0x40];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k<16);return pv[mon(p)][k]; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,NULL,NULL};
static uint32_t last_move(void *u,uint32_t p) { (void)u;notef("lm%u ",mon(p),0);return lm_value; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%x ",mon(p),s);return hs_value; }
static uint32_t encore_blocked(void *u,uint32_t m) { (void)u;notef("eb%x ",m,0);return eb_value; }
static uint32_t has_move(void *u,uint32_t p,uint32_t m) { (void)u;notef("hm%u/%x ",mon(p),m);return hm_value; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",mon(p),f);return tf_value; }
static uint32_t condition_word(void *u,uint32_t a,uint32_t b) { (void)u;notef("cw%x/%x ",a,b);return 0xa0000000u+a*0x100+b; }
static uint32_t transformed(void *u,uint32_t p) { (void)u;notef("tr%u ",mon(p),0);return tr_value; }
static uint32_t copy_source(void *u,uint32_t p) { (void)u;notef("cs%u ",mon(p),0);return cs_value; }
static uint32_t mimic_blocked(void *u,uint32_t m) { (void)u;notef("mb%x ",m,0);return mb_value; }
static uint32_t move_slot(void *u,uint32_t p,uint32_t m) { (void)u;notef("ms%u/%x ",mon(p),m);return m==0x55?slot_own:slot_new; }
static uint32_t hp_share(void *u,uint32_t p,uint32_t d) { (void)u;notef("hp%u/%u ",mon(p),d);return cost_value; }
static uint32_t stage_room(void *u,uint32_t p,uint32_t s) { (void)u;notef("sr%u/%u ",mon(p),s);return room_value; }
static uint32_t context_word(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("cx ");return 0x77665544u; }
static uint32_t raw_stat(void *u,uint32_t p,uint32_t k) { (void)u;notef("rs%u/%x ",mon(p),k);CHECK(k<16);return rs[mon(p)][k]; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX);notef("tb%x ",n,0);return BUF; }
static uint32_t resist_types(void *u,uint32_t t,uint32_t out) { (void)u;notef("rt%x ",t,0);CHECK(out==BUF);
    for(uint32_t i=0;i<count_value;++i){w8(NULL,out+i,list_values[i]);}
    return count_value; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ht%u/%x ",mon(p),t);return (has_mask>>t)&1; }
static uint32_t random_(void *u,uint32_t n) { (void)u;notef("rn%u ",n,0);return rnd_value%n; }
static void move_bytes(void *u,uint32_t d,uint32_t s,uint32_t n) { (void)u;notef("mv%x/",d-BUF,0);notef("%x/",s-BUF,0);notef("%u ",n,0);memmove(mem+d,mem+s,n); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x40);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x/%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<12);memcpy(commits[ncommit++],mem+RECORD,0x40);note("commit "); }
static const struct px_battle_moves_b4_api api={&base,last_move,has_status,encore_blocked,has_move,turn_flag,condition_word,transformed,
    copy_source,mimic_blocked,move_slot,hp_share,stage_room,context_word,raw_stat,temp_buffer,resist_types,has_type,random_,move_bytes,
    action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b4_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_belly_drum_effect,px_battle_shell_smash_effect};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);memset(pv,0,sizeof pv);memset(rs,0,sizeof rs);aborts=ncommit=0;log_[0]=0;
    lm_value=hs_value=eb_value=hm_value=tf_value=tr_value=cs_value=mb_value=slot_new=slot_own=cost_value=room_value=0;
    has_mask=rnd_value=count_value=0;memset(list_values,0,sizeof list_values);
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t k3,uint32_t target)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_id(0x06,target,&events);log_[0]=0;
}
static void run(fn f,uint32_t k3,uint32_t target)
{
    frame(k3,target);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0);
}
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
/* Bytes of commit n other than those listed keep the 0xa5 fill; the first word is 'head'. */
static void untouched(unsigned n,const unsigned char *keep,unsigned count,uint32_t head)
{
    for(unsigned o=4;o<0x40;++o) {
        int skip=0;for(unsigned i=0;i<count;++i)if(keep[i]==o)skip=1;
        if(skip)continue;
        if(commits[n][o]!=0xa5){fprintf(stderr,"commit %u byte %x = %x\n",n,o,commits[n][o]);CHECK(0);}
    }
    CHECK(c32(n,0)==head);
}
#define KEEP(...) (const unsigned char[]){__VA_ARGS__}
#define UNT(n,head,...) untouched(n,KEEP(__VA_ARGS__),sizeof KEEP(__VA_ARGS__),head)
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
static void appendf(char *s,size_t n,const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);strncat(s,t,n-strlen(s)-1); }
int main(void)
{
    unsigned cases=0;char want[1024];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    static const uint32_t targets[3]={OTHER,1,0x103};
    /* Encore. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t)for(unsigned f=0;f<16;++f)for(unsigned m=0;m<2;++m) {
        static const uint32_t moves[2]={0x33,0x1a5};
        reset();hs_value=f&1;eb_value=f>>1&1;hm_value=!(f>>2&1);tf_value=f>>3&1;lm_value=moves[m];
        run(px_battle_encore_effect,ids[a],targets[t]);const uint32_t tg=targets[t]&0xff;
        want[0]=0;
        if(ids[a]==ME){
            appendf(want,sizeof want,"lm%u hs%u/17 ",tg,tg);
            if(!hs_value){appendf(want,sizeof want,"eb%x ",moves[m],0);
              if(!eb_value){appendf(want,sizeof want,"hm%u/%x ",tg,moves[m]);
                if(hm_value){appendf(want,sizeof want,"tf%u/1 ",tg,0);appendf(want,sizeof want,"recb/2 cw%x/",tf_value?4:3,0);
                  appendf(want,sizeof want,"%x msg22f/142 ",moves[m],0);appendf(want,sizeof want,"arg%x/14 commit ",tg,0);
                  CHECK(c16(0,4)==0x17&&c32(0,8)==0xa0000000u+(tf_value?4:3)*0x100+moves[m]&&commits[0][0xf]==tg&&commits[0][0xc]==0);
                  EXPECT(want);UNT(0,0x11,4,5,8,9,10,11,0xc,0xf);++cases;continue;}}}
        }
        EXPECT(want);++cases;
    }
    /* Mimic. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t)for(unsigned f=0;f<16;++f)for(unsigned cs=0;cs<3;++cs)for(unsigned l=0;l<3;++l) {
        static const uint32_t srcs[3]={0,0x11,0x10022},lasts[3]={0,0x11,0x33};
        reset();w16(NULL,HANDLER+0x38,0x55);tr_value=f&1;mb_value=f>>1&1;slot_new=(f>>2&1)?4:3;slot_own=(f>>3&1)?4:1;
        cs_value=srcs[cs];lm_value=lasts[l];run(px_battle_mimic_effect,ids[a],targets[t]);
        const uint32_t tg=targets[t]&0xff;const uint32_t mv=lasts[l]!=srcs[cs]?lasts[l]:srcs[cs];
        want[0]=0;
        if(ids[a]==ME){
            note("");appendf(want,sizeof want,"tr%u ",ME,0);
            if(!tr_value){appendf(want,sizeof want,"cs%u ",tg,0);appendf(want,sizeof want,"lm%u ",tg,0);
              if(mv){appendf(want,sizeof want,"mb%x ",mv,0);
                if(!mb_value){appendf(want,sizeof want,"ms2/%x ",mv,0);
                  if(slot_new==4){appendf(want,sizeof want,"ms2/%x ",0x55,0);
                    if(slot_own!=4){appendf(want,sizeof want,"rec26/2 commit rec3/2 msg2b0/42 arg2/4 arg%x/4 ",mv,0);strcat(want,"commit ");
                      CHECK(1);}}}}}
        }
        EXPECT(want);
        if(ids[a]==ME&&!tr_value&&mv&&!mb_value&&slot_new==4&&slot_own!=4){
            CHECK(commits[0][4]==ME&&commits[0][5]==slot_own&&commits[0][6]==0&&commits[0][7]==0&&c16(0,8)==(mv&0xffff));UNT(0,0x11,4,5,6,7,8,9);}
        ++cases;
    }
    /* Conversion 2. */
    for(unsigned a=0;a<3;++a)for(unsigned ty=0;ty<3;++ty)for(unsigned l=0;l<3;++l)for(unsigned h=0;h<4;++h)for(unsigned r=0;r<3;++r) {
        static const uint32_t tys[3]={0x12,5,0x11};
        static const uint8_t lists[3][4]={{0,0,0,0},{3,7,0,0},{1,2,3,4}};static const uint32_t counts[3]={0,2,4};
        static const uint32_t masks[4]={0,0x8,0x1e,0xffff};static const uint32_t rnds[3]={0,1,5};
        reset();w8(NULL,MON+0x100*1+0x157,(uint8_t)tys[ty]);count_value=counts[l];memcpy(list_values,lists[l],4);has_mask=masks[h];rnd_value=rnds[r];
        run(px_battle_conversion_2_effect,ids[a],1);
        want[0]=0;
        if(ids[a]==ME&&tys[ty]!=0x12){
            appendf(want,sizeof want,"tb12 rt%x ",tys[ty],0);
            if(counts[l]){
                uint8_t cur[4];memcpy(cur,lists[l],4);uint32_t n=counts[l],i=0;
                while(i<n){const uint32_t ty2=cur[i];appendf(want,sizeof want,"ht%u/",ME,0);appendf(want,sizeof want,"%x ",ty2,0);
                    if(!((masks[h]>>ty2)&1)){++i;continue;}
                    const int rest=(int)(n-i)-1;
                    if(rest>0){appendf(want,sizeof want,"mv%x/",i,0);appendf(want,sizeof want,"%x/",i+1,0);appendf(want,sizeof want,"%u ",(uint32_t)rest,0);memmove(cur+i,cur+i+1,(size_t)rest);}
                    --n;}
                if(n){appendf(want,sizeof want,"rn%u ",n,0);const uint32_t k=rnds[r]%n,chosen=cur[k];strcat(want,"rec13/2 commit ");
                    CHECK(1);
                    EXPECT(want);CHECK(commits[0][6]==ME&&c16(0,4)==((chosen&0x1f)|(chosen&0x1f)<<5|0x4800u));UNT(0,0x11,4,5,6);++cases;continue;}
            }
        }
        EXPECT(want);CHECK(ncommit==0);++cases;
    }
    /* Belly Drum. */
    for(unsigned a=0;a<3;++a)for(unsigned hp=0;hp<4;++hp)for(unsigned rm=0;rm<3;++rm) {
        static const uint32_t hps[4]={4,5,6,0xffffffffu},rooms[3]={0,3,0x1fe};
        reset();cost_value=5;room_value=rooms[rm];pv[ME][0xd]=hps[hp];run(px_battle_belly_drum_effect,ids[a],OTHER);
        const int on=ids[a]==ME&&hps[hp]>5&&rooms[rm]!=0;
        EXPECT(ids[a]!=ME?"":on?"hp2/2 sr2/1 v2/d rec7/2 commit recd/2 commit rec3/2 msg265/42 arg2/4 commit rec22/2 commit "
                              :"hp2/2 sr2/1 v2/d ");
        if(on){CHECK(commits[0][4]==1&&c32(0,0x14)==0xfffffffbu&&commits[0][6]==1&&commits[0][7]==ME);UNT(0,0x11,4,6,7,0x14,0x15,0x16,0x17);
            CHECK(commits[1][4]==1&&commits[1][0xc]==ME&&commits[1][0x17]==(rooms[rm]&0xff)&&commits[1][0x16]==1&&commits[1][0x18]==0xa7);
            UNT(1,0x11,4,0xc,0x16,0x17,0x18);CHECK(commits[3][4]==ME&&commits[3][5]==1);UNT(3,0x11,4,5);}
        ++cases;
    }
    /* Pain Split. */
    for(unsigned a=0;a<3;++a)for(unsigned x=0;x<4;++x)for(unsigned y=0;y<4;++y)for(unsigned t=0;t<2;++t) {
        static const uint32_t hps[4]={0,7,0x80000000u,0xffffffffu};
        reset();pv[ME][0xd]=hps[x];pv[targets[t]&0xff][0xd]=hps[y];run(px_battle_pain_split_effect,ids[a],targets[t]);
        const uint32_t tg=targets[t]&0xff,avg=(hps[x]+hps[y])>>1;
        snprintf(want,sizeof want,"v2/d v%u/d rec7/2 commit rec3/2 msg75/41 commit rec22/2 commit rec22/2 commit ",tg);
        EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){CHECK(commits[0][4]==2&&c32(0,0x14)==avg-hps[x]&&c32(0,0x18)==avg-hps[y]&&commits[0][6]==1&&commits[0][7]==ME&&commits[0][8]==tg);
            UNT(0,0x11,4,6,7,8,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b);
            CHECK(commits[2][4]==ME&&commits[2][5]==1&&commits[3][4]==tg&&commits[3][5]==1);UNT(2,0x11,4,5);UNT(3,0x11,4,5);}
        ++cases;
    }
    /* Memento. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t) {
        reset();run(px_battle_memento_effect,ids[a],targets[t]);
        EXPECT(ids[a]==ME?"cx recd/2 commit recd/2 commit rec12/2 commit ":"");
        if(ids[a]==ME)for(unsigned i=0;i<2;++i){
            CHECK(commits[i][0xc]==(targets[t]&0xff)&&commits[i][0x16]==1&&commits[i][4]==(i?3u:1u)&&commits[i][0x17]==0xfe&&commits[i][0x18]==0xb7&&c32(i,8)==0x77665544u);
            UNT(i,0x11,4,8,9,10,11,0xc,0x16,0x17,0x18);}
        if(ids[a]==ME){CHECK(commits[2][4]==ME);UNT(2,0x11,4);}
        ++cases;
    }
    /* Guard Split and Power Split. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t)for(unsigned v=0;v<4;++v)for(unsigned g=0;g<2;++g) {
        static const uint32_t s1[4]={10,0xfffffff9u,3,0x7fffffffu},s2[4]={21,4,0xfffffffeu,0x7fffffffu};
        reset();const uint32_t tg=targets[t]&0xff;const uint32_t k1=g?9:8,k2=g?0xb:0xa;
        rs[ME][k1]=s1[v];rs[tg][k1]=s2[v];rs[ME][k2]=s2[v];rs[tg][k2]=s1[v]+1;
        run(g?px_battle_guard_split_effect:px_battle_power_split_effect,ids[a],targets[t]);
        const uint32_t x=(uint32_t)((int32_t)(s1[v]+s2[v])/2),y=(uint32_t)((int32_t)(s2[v]+s1[v]+1)/2);
        snprintf(want,sizeof want,"rs2/%x rs%u/%x rs2/%x rs%u/%x rec10/2 commit rec10/2 msg%x/102 arg2/10 commit ",k1,tg,k1,k2,tg,k2,g?0x451u:0x44eu);
        EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){
            const unsigned o1=g?6:4,o2=g?0xa:8;const unsigned flags=g?0xa:5;
            for(unsigned i=0;i<2;++i){CHECK(c16(i,o1)==(x&0xffff)&&c16(i,o2)==(y&0xffff)&&commits[i][0xe]==(i?(uint32_t)ME:tg)&&commits[i][0xf]==(0xa5|flags));
                const unsigned char keep[]={(unsigned char)o1,(unsigned char)(o1+1),(unsigned char)o2,(unsigned char)(o2+1),0xe,0xf};
                untouched(i,keep,sizeof keep,0x11);}
        }
        ++cases;
    }
    /* Topsy-Turvy. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t)for(unsigned v=0;v<4;++v) {
        static const uint32_t sets[4][7]={{6,6,6,6,6,6,6},{6,7,5,6,0,12,6},{7,6,6,6,6,6,6},{0x106,6,6,6,6,6,0xff}};
        reset();const uint32_t tg=targets[t]&0xff;for(unsigned i=0;i<7;++i)pv[tg][i+1]=sets[v][i];w8(NULL,MON+0x100*tg+0x155,0x9c);
        run(px_battle_topsy_turvy_effect,ids[a],targets[t]);
        want[0]=0;int changed=0;
        if(ids[a]==ME){strcat(want,"tb1c ");for(unsigned i=0;i<7;++i){appendf(want,sizeof want,"v%u/",tg,0);appendf(want,sizeof want,"%x ",i+1,0);if(sets[v][i]!=6)changed=1;}
            if(changed){appendf(want,sizeof want,"rece/2 commit rec3/2 msg499/42 arg%x/4 commit ",tg,0);}}
        EXPECT(want);
        if(ids[a]==ME&&changed){
            CHECK(commits[0][4]==tg&&commits[0][0xc]==0x9c);for(unsigned i=0;i<7;++i){
                const uint32_t out=sets[v][i]==6?6u:6u-(sets[v][i]-6u);CHECK(commits[0][5+i]==(out&0xff)&&r32(NULL,BUF+4*i)==out);}
            UNT(0,0x11,4,5,6,7,8,9,10,11,0xc);}
        ++cases;
    }
    /* Shell Smash. */
    for(unsigned a=0;a<3;++a) {
        reset();run(px_battle_shell_smash_effect,ids[a],OTHER);
        EXPECT(ids[a]==ME?"recd/2 commit recd/2 commit recd/2 commit recd/2 commit recd/2 commit ":"");
        if(ids[a]==ME)for(unsigned i=0;i<5;++i){
            static const uint8_t stats[5]={2,4,1,3,5},stages[5]={0xff,0xff,2,2,2};
            CHECK(commits[i][4]==stats[i]&&commits[i][0x17]==stages[i]&&commits[i][0x16]==1&&commits[i][0x18]==0xa7&&commits[i][0xc]==ME);
            UNT(i,0x11,4,0xc,0x16,0x17,0x18);}
        ++cases;
    }
    /* Through the dispatcher: Belly Drum then Shell Smash on event 0xac. */
    {
        reset();cost_value=5;room_value=6;pv[ME][0xd]=40;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xac);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(ME,OTHER);px_battle_event_dispatch(CTX,0xac,&events);
        EXPECT("hp2/2 sr2/1 v2/d rec7/2 commit recd/2 commit rec3/2 msg265/42 arg2/4 commit rec22/2 commit recd/2 commit recd/2 commit recd/2 commit recd/2 commit recd/2 commit ");
        CHECK(aborts==0&&ncommit==9);++cases;
    }
    printf("battle_moves_b4: %u cases, all tests passed\n",cases);
    return 0;
}
