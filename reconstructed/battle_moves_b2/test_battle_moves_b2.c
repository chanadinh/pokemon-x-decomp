#include "battle_moves_b2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t mode_value,last_value,has_value,status_value,side_value,move_value,type_value,flags_value;
static unsigned aborts,ncommit;static uint8_t commits[4][0x40];static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("st%u ",mon(p),0);return status_value; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0);return side_value; }
static const struct px_battle_ability_api base={&events,NULL,NULL,pokemon_status,side_of,NULL,NULL};
static uint32_t battle_mode(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("mode ");return mode_value; }
static void header_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("hdr%x/%u ",k,id); }
static void protect_start(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("protect%u ",id,0); }
static uint32_t moves_last(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("last%u ",id,0);return last_value; }
static uint32_t imprison_condition(void *u,uint32_t id) { (void)u;notef("cond%u ",id,0);return 0xc0de1234u+id; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("type%u/%x ",mon(p),t);return has_value; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("has%u/%x ",mon(p),s);return has_value; }
static uint32_t move_at(void *u,uint32_t p,uint32_t s) { (void)u;notef("moveat%u/%u ",mon(p),s);return move_value; }
static uint32_t move_type(void *u,uint32_t m) { (void)u;notef("mtype%x ",m,0);return type_value; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x40);w32(NULL,RECORD,0x11);w16(NULL,RECORD+0xe,(uint16_t)flags_value);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x/%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x40);note("commit "); }
static const struct px_battle_moves_b2_api api={&base,battle_mode,header_record,protect_start,moves_last,imprison_condition,
    has_type,has_status,move_at,move_type,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b2_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_grudge_effect,px_battle_follow_me_effect,px_battle_magic_coat_effect};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    mode_value=last_value=has_value=status_value=side_value=move_value=type_value=flags_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* Key 0x03 the user, 0x05 the target count, 0x06.. the targets, 0x53 a plain value. */
static void frame(uint32_t k3,uint32_t count,const uint32_t *targets)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_value(0x05,count,&events);
    for(unsigned i=0;i<3;++i)px_battle_event_set_id(0x06+i,targets[i],&events);
    px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static const uint32_t none[3]={OTHER,4,6};
static void run(fn f,uint32_t k3) { frame(k3,1,none);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
/* Bytes of a committed record other than those listed keep the 0xa5 fill (the header word 0x11). */
static void untouched(unsigned n,const char *keep)
{
    for(unsigned o=4;o<0x40;++o){ if(o==0xe||o==0xf||strchr(keep,(char)(o+1)))continue;
        if(commits[n][o]!=0xa5){fprintf(stderr,"commit %u byte %x = %x\n",n,o,commits[n][o]);CHECK(0);} }
    CHECK(c32(n,0)==0x11);
}
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;char want[256];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    static const uint32_t work_in[3]={0,1,0x80000000u};
    /* Teleport: the running handler is marked for re-entry, then kind 0x29. */
    for(unsigned a=0;a<3;++a)for(unsigned fl=0;fl<4;++fl) {
        static const uint32_t flags[4]={0,0x1000000,0x1234567,0x20000000};
        reset();w32(NULL,HANDLER+0x18,flags[fl]);run(px_battle_teleport_effect,ids[a]);
        const int on=ids[a]==ME;EXPECT(on?"hdr29/2 ":"");
        CHECK(r32(NULL,HANDLER+0x18)==(on&&(flags[fl]&0x1000000)?flags[fl]|0x10000000:flags[fl]));++cases;
    }
    /* Hold Hands: modes 1 and 2 only; key 0x53 rewritten, kind 0x3c (also when 0x53 was already rewritten). */
    for(unsigned a=0;a<3;++a)for(uint32_t md=0;md<5;++md)for(unsigned pre=0;pre<2;++pre) {
        const uint32_t m=md==4?0x101:md;
        reset();mode_value=m;frame(ids[a],1,none);if(pre)px_battle_event_rewrite(0x53,7,&events);log_[0]=0;
        px_battle_hold_hands_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        const int on=ids[a]==ME&&(m==1||m==2);
        EXPECT(ids[a]!=ME?"":on?"mode hdr3c/2 ":"mode ");CHECK(get(0x53)==(pre?7u:0u));
        if(!pre){CHECK(px_battle_event_rewrite(0x53,9,&events)==(on?0u:1u));}
        ++cases;
    }
    /* Wish: kind 0x1f, field 0 and the user's position byte. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<3;++s) {
        static const uint32_t sides[3]={0,3,0x1a7};
        reset();side_value=sides[s];run(px_battle_wish_effect,ids[a]);
        const int on=ids[a]==ME;EXPECT(on?"rec1f/2 side2 commit ":"");
        if(on){CHECK(commits[0][4]==0&&commits[0][5]==(sides[s]&0xff));untouched(0,"\5\6");}
        ++cases;
    }
    /* King's Shield, Grudge, Follow Me, Magic Coat and Snatch: kind 0x15 or a message, work word 6 set. */
    for(unsigned a=0;a<3;++a)for(unsigned wi=0;wi<3;++wi)for(unsigned l=0;l<3;++l) {
        static const uint32_t lasts[3]={0,1,0x100};
        const int me=ids[a]==ME;
        reset();w32(NULL,WORK+0x18,work_in[wi]);run(px_battle_kings_shield_effect,ids[a]);
        EXPECT(me?"protect2 rec15/2 commit ":"");CHECK(r32(NULL,WORK+0x18)==(me?1u:work_in[wi]));
        if(me){CHECK(commits[0][4]==0x10&&commits[0][5]==ME);untouched(0,"\5\6");}
        static const struct { fn f; uint32_t msg; } said[2]={{px_battle_grudge_effect,0x278},{px_battle_follow_me_effect,0x29e}};
        for(unsigned i=0;i<2;++i) {
            reset();w32(NULL,WORK+0x18,work_in[wi]);run(said[i].f,ids[a]);
            snprintf(want,sizeof want,"rec3/2 msg%x/42 arg2/4 commit ",said[i].msg);
            EXPECT(me?want:"");CHECK(r32(NULL,WORK+0x18)==(me?1u:work_in[wi]));if(me)untouched(0,"");
        }
        reset();w32(NULL,WORK+0x18,work_in[wi]);run(px_battle_magic_coat_effect,ids[a]);
        const int coat=me&&work_in[wi]==0;
        EXPECT(coat?"rec3/2 msg2f9/42 arg2/4 commit ":"");CHECK(r32(NULL,WORK+0x18)==(coat?1u:work_in[wi]));
        reset();w32(NULL,WORK+0x18,work_in[wi]);last_value=lasts[l];run(px_battle_snatch_effect,ids[a]);
        const int snatch=me&&lasts[l]==0;
        EXPECT(!me?"":snatch?"last2 rec3/2 msg2ef/42 arg2/4 commit ":"last2 ");CHECK(r32(NULL,WORK+0x18)==(snatch?1u:work_in[wi]));
        cases+=5;
    }
    /* Quash and Transform: kind 0x32 or 0x34 with the target's id byte. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t) {
        static const uint32_t targets[3]={OTHER,0,0x104};
        const uint32_t tg[3]={targets[t],1,3};const int on=ids[a]==ME;
        reset();frame(ids[a],1,tg);px_battle_quash_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        snprintf(want,sizeof want,"rec32/2 msg471/82 arg%x/8 commit ",targets[t]&0xff);EXPECT(on?want:"");
        if(on){CHECK(commits[0][4]==(targets[t]&0xff));untouched(0,"\5");}
        reset();frame(ids[a],1,tg);px_battle_transform_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        snprintf(want,sizeof want,"rec34/2 msg284/82 arg2/8 arg%x/8 commit ",targets[t]&0xff);EXPECT(on?want:"");
        if(on){CHECK(commits[0][4]==(targets[t]&0xff));untouched(0,"\5");}
        cases+=2;
    }
    /* Refresh: poison, paralysis or burn is cured. */
    for(unsigned a=0;a<3;++a)for(uint32_t s=0;s<9;++s) {
        const uint32_t st=s==8?0x104:s;
        reset();status_value=st;run(px_battle_refresh_effect,ids[a]);
        const int on=ids[a]==ME&&(st==5||st==1||st==4);
        EXPECT(ids[a]!=ME?"":on?"st2 reca/2 commit ":"st2 ");
        if(on){CHECK(commits[0][4]==st&&commits[0][5]==ME&&commits[0][0x11]==1);untouched(0,"\5\6\22");}
        ++cases;
    }
    /* Forest's Curse and Torment: the target (low byte of key 0x06) unless it already has Grass / Torment. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t)for(unsigned h=0;h<3;++h) {
        static const uint32_t targets[3]={OTHER,1,0x103},has[3]={0,1,0x100};
        const uint32_t tg[3]={targets[t],4,6},target=targets[t]&0xff;const int me=ids[a]==ME,on=me&&has[h]==0;
        reset();has_value=has[h];frame(ids[a],1,tg);px_battle_forests_curse_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        snprintf(want,sizeof want,on?"type%u/b rec14/2 commit ":"type%u/b ",target);EXPECT(me?want:"");
        if(on){CHECK(commits[0][4]==0xb&&commits[0][5]==target);untouched(0,"\5\6");}
        reset();has_value=has[h];frame(ids[a],1,tg);px_battle_torment_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        snprintf(want,sizeof want,on?"has%u/c recb/2 commit ":"has%u/c ",target);EXPECT(me?want:"");
        if(on){CHECK(c16(0,4)==0xc&&c32(0,8)==1&&commits[0][0xf]==target);untouched(0,"\5\6\11\12\13\14");}
        cases+=2;
    }
    /* Magnet Rise: condition 0x1e for five turns. */
    for(unsigned a=0;a<3;++a) {
        reset();run(px_battle_magnet_rise_effect,ids[a]);
        const int on=ids[a]==ME;EXPECT(on?"recb/2 msg292/142 arg2/14 commit ":"");
        if(on){CHECK(c16(0,4)==0x1e&&c32(0,8)==px_battle_turn_count(5)&&c32(0,8)==0x2a&&commits[0][0xf]==ME);
            untouched(0,"\5\6\11\12\13\14");}
        ++cases;
    }
    /* Imprison: kind 0x1c; the top two bits of +0xe are kept. */
    for(unsigned a=0;a<3;++a)for(unsigned f=0;f<5;++f) {
        static const uint32_t flags[5]={0,0x3fff,0x4000,0x8001,0xffff};
        reset();flags_value=flags[f];run(px_battle_imprison_effect,ids[a]);
        const int on=ids[a]==ME;EXPECT(on?"rec1c/2 cond2 commit ":"");
        if(on){CHECK(commits[0][4]==3&&c32(0,8)==0xc0de1236u&&c16(0,0xc)==0x24a&&c16(0,0xe)==((flags[f]&0xc000)|0x82)&&
            c32(0,0x10)==ME&&commits[0][0x30]==1);untouched(0,"\5\11\12\13\14\15\16\21\22\23\24\61");}
        ++cases;
    }
    /* Soak: one pure-Water record per target. */
    for(unsigned a=0;a<3;++a)for(uint32_t count=0;count<4;++count) {
       static const uint32_t tg[3]={OTHER,0x101,0};
        reset();frame(ids[a],count,tg);
        px_battle_soak_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        const unsigned k=ids[a]==ME?count:0;
        CHECK(ncommit==k);
        for(unsigned i=0;i<k;++i){CHECK(c16(i,4)==0x494a&&commits[i][6]==(tg[i]&0xff)&&commits[i][8]==1);untouched(i,"\5\6\7\11");}
        want[0]=0;for(unsigned i=0;i<k;++i)strcat(want,"rec13/2 commit ");EXPECT(want);
        ++cases;
    }
    /* Soak past the frame's targets: key 0x09 is missing, the battle aborts. */
    { static const uint32_t tg[3]={OTHER,1,3};reset();frame(ME,4,tg);px_battle_soak_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts>=1&&ncommit==4);++cases; }
    /* Conversion: the user takes its first move's type unless it has it. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<4;++t)for(unsigned h=0;h<2;++h) {
        static const uint32_t types[4]={0,0xa,0x11,0x1e5};
        reset();move_value=0x1234+t;type_value=types[t];has_value=h;run(px_battle_conversion_effect,ids[a]);
        const int me=ids[a]==ME,on=me&&!h;
        snprintf(want,sizeof want,"moveat2/0 mtype%x type2/%x ",0x1234+t,types[t]);if(on)strcat(want,"rec13/2 commit ");EXPECT(me?want:"");
        if(on){const uint32_t ty=types[t]&0x1f;CHECK(c16(0,4)==(ty|ty<<5|0x4800)&&commits[0][6]==ME);untouched(0,"\5\6\7");}
        ++cases;
    }
    /* Through the dispatcher: Grudge, Follow Me and Magic Coat on event 0xac (Magic Coat's work word 6 already set). */
    {
        reset();
        for(unsigned i=0;i<3;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xac);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<2?h+0x40:0);
        }
        w32(NULL,HANDLER+0x80+0x1c+0x18,1);w32(NULL,BATTLE+0x60,HANDLER);frame(ME,1,none);px_battle_event_dispatch(CTX,0xac,&events);
        EXPECT("rec3/2 msg278/42 arg2/4 commit rec3/2 msg29e/42 arg2/4 commit ");CHECK(aborts==0&&ncommit==2);++cases;
    }
    printf("battle_moves_b2: %u cases, all tests passed\n",cases);
    return 0;
}
