#include "battle_low_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, TABLE=0x3100, MON=0x4000, CTX=0x3000, RNG=0x9000,
       OBJ=0xa000, ITER=0xb000, ENTRIES=0xb100, HANDLER=0x6000, WORK=0x5000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,handler_keys;static char log_[2048];
static uint32_t present_mask,owner_value,pp_value,roll_value,hp_value,allow_value,score[8],entry_count,entry_pos,entry_ptrs[8];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[96];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%x/%x ",p,k);return hp_value; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,NULL,NULL};
static uint32_t slot_present(void *u,uint32_t o,uint32_t i) { (void)u;notef("sp%x/%u ",o,i);return present_mask>>i&1; }
static uint32_t owner_of(void *u,uint32_t id) { (void)u;notef("ow%x ",id,0);return owner_value; }
static uint32_t entry_score(void *u,uint32_t c,uint32_t e,uint32_t a) { (void)u;(void)c;notef("sc%x/%x ",e,a);
    for(unsigned i=0;i<8;++i){if(entry_ptrs[i]==e)return score[i];}
    return score[7]; }
static void entry_first(void *u,uint32_t c,uint32_t it) { (void)u;(void)c;CHECK(it==ITER);note("first ");entry_pos=0; }
static uint32_t entry_next(void *u,uint32_t c,uint32_t it,uint32_t out) { (void)u;(void)c;CHECK(it==ITER&&out==ITER+4);
    if(entry_pos>=entry_count){return 0;}
    w32(NULL,out,entry_ptrs[entry_pos++]);return 1; }
static void record_event(void *u,uint32_t w,uint32_t k,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e) { (void)u;
    char t[160];snprintf(t,sizeof t,"ev%x/%x/%x/%x/%x/%x/%x ",w,k,a,b,c,d,e);note(t); }
static void queue_change(void *u,uint32_t c,uint32_t b,uint32_t k,uint32_t f) { (void)u;char t[96];snprintf(t,sizeof t,"q%x/%x/%x/%x ",c,b,k,f);note(t); }
static uint32_t random_below(void *u,uint32_t s,uint32_t l) { (void)u;notef("rb%x/",s,0);notef("%u ",l,0);return roll_value; }
static uint32_t base_pp(void *u,uint32_t m,uint32_t f) { (void)u;notef("pp%x/",m,0);notef("%x ",f,0);return pp_value; }
static void commit_slot(void *u,uint32_t s,uint32_t r) { (void)u;notef("cs%x/",s,0);notef("%x ",r,0); }
static const struct px_battle_low_api api={&base,slot_present,owner_of,entry_score,entry_first,entry_next,record_event,queue_change,random_below,
    base_pp,commit_slot,RNG,ITER};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)w;CHECK(f==0);
    if(handler_keys==0){notef("h2=%x ",px_battle_event_get(0x02,&events),0);}
    else{notef("h3=%x ",px_battle_event_get(0x03,&events),0);notef("h4=%x ",px_battle_event_get(0x04,&events),0);}
    (void)v;
    if(allow_value)px_battle_event_rewrite(0x53,1,&events);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;handler_keys=0;log_[0]=0;present_mask=owner_value=pp_value=roll_value=hp_value=allow_value=entry_count=entry_pos=0;
    memset(score,0,sizeof score);memset(entry_ptrs,0,sizeof entry_ptrs);
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,i==7?0:MON+0x100*i);
}
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
static unsigned long long lcg_state=0x1234567;
static uint32_t rnd(void) { lcg_state=lcg_state*6364136223846793005ULL+1442695040888963407ULL;return (uint32_t)(lcg_state>>33); }
static void fill_noise(void) { for(unsigned i=0;i<0x400;++i)mem[0xc000+i]=(uint8_t)(0xa5^i); }
int main(void)
{
    unsigned long cases=0;char want[2048];
    /* 10814f9c, 1081cc04: pure values. */
    for(uint32_t v=0;v<12;++v){CHECK(px_battle_is_mode_2_or_3(v)==(v==2||v==3));++cases;}
    CHECK(px_battle_is_mode_2_or_3(0x100000002u&0xffffffffu)==1&&px_battle_is_mode_2_or_3(0x80000002u)==0&&px_battle_is_mode_2_or_3(0xffffffffu)==0);
    for(uint32_t c=0;c<0x30000;c+=0x1111){CHECK(px_battle_context_0ca4(c)==c+0xca4);++cases;}
    CHECK(px_battle_context_0ca4(0xfffff000u)==0xfffff000u+0xca4u);
    /* 1081cf08: the halfword at +0x4674. */
    for(unsigned i=0;i<64;++i){reset();const uint32_t c=0x1000+i*0x10;w16(NULL,c+0x4674,(uint16_t)rnd());w32(NULL,c+0x4670,0x11111111u);w32(NULL,c+0x4678,0x22222222u);
        CHECK(px_battle_half_4674(c,&memory)==r16(NULL,c+0x4674));++cases;}
    /* 1081d83c: the counter. */
    for(unsigned i=0;i<7;++i){static const uint32_t starts[7]={0,1,5,0x7fffffff,0xfffffffe,0xffffffff,0x80000000u};
        reset();const uint32_t c=0x2000;w32(NULL,c+0x1084,starts[i]);w32(NULL,c+0x1080,0xdeadbeef);w32(NULL,c+0x1088,0xcafef00d);
        const uint32_t exp=starts[i]+1==0?1:starts[i]+1;CHECK(px_battle_next_counter(c,&memory)==exp&&r32(NULL,c+0x1084)==exp&&r32(NULL,c+0x1080)==0xdeadbeef&&r32(NULL,c+0x1088)==0xcafef00d);++cases;}
    /* 1081d748: append to the byte list (at most 10). */
    for(unsigned n=0;n<14;++n)for(unsigned k=0;k<3;++k){static const uint32_t vals[3]={0x41,0x1ff,0xffffffffu};
        reset();const uint32_t c=0x2000;fill_noise();w8(NULL,c+0x1091,(uint8_t)n);for(unsigned i=0;i<16;++i)w8(NULL,c+0x1094+i,(uint8_t)(0x30+i));
        px_battle_push_byte(c,vals[k],&memory);
        const unsigned on=n<10;CHECK(r8(NULL,c+0x1091)==(on?n+1:n));
        for(unsigned i=0;i<16;++i)CHECK(r8(NULL,c+0x1094+i)==(on&&i==n?(vals[k]&0xff):(0x30+i)));
        ++cases;}
    /* 1081ca98: add with a cap. */
    for(unsigned st=0;st<4;++st)for(unsigned fl=0;fl<4;++fl)for(unsigned b=0;b<7;++b)for(unsigned am=0;am<6;++am){
        static const uint32_t states[4]={0,1,2,0xff},flags[4]={0,8,0xf7,0xff},bases[7]={0,5,99990,99999,100000,0xffffff00u,0x1869f},amounts[6]={0,1,9,10,0x10000,0xffffff00u};
        reset();const uint32_t T=0xa800,S=0xa900,O=0xa000,A=0xa200,B=0xa400;
        w32(NULL,O,A);w32(NULL,O+4,B);w32(NULL,A+0x10,T);w32(NULL,B+0x10,S);w8(NULL,S,(uint8_t)states[st]);
        w8(NULL,T+0x81c,(uint8_t)flags[fl]);w32(NULL,T+0x804,bases[b]);
        const uint32_t r=px_battle_add_capped(O,amounts[am],&memory);
        const int ok=states[st]<=1;uint32_t total=bases[b]+amounts[am];if(total>99999)total=99999;
        CHECK(r==(ok?1u:0u));CHECK(r32(NULL,T+0x804)==(ok&&!(flags[fl]&8)?total:bases[b]));++cases;}
    /* 10810cc0: add a value unless present (list of at most 6). */
    for(unsigned n=0;n<8;++n)for(unsigned k=0;k<5;++k){
        static const uint32_t vals[5]={0x41,0x42,0x141,0xff,0};
        reset();const uint32_t c=0x2000;fill_noise();w8(NULL,c+0x18a6,(uint8_t)n);for(unsigned i=0;i<8;++i)w8(NULL,c+0x189c+i,(uint8_t)(0x40+i%3));
        px_battle_add_unique(c,vals[k],&api);
        int present=0;for(unsigned i=0;i<n;++i)if((uint32_t)(0x40+i%3)==vals[k])present=1;
        CHECK(aborts==(n>=6?1u:0u));
        CHECK(r8(NULL,c+0x18a6)==(present?n:n+1));
        for(unsigned i=0;i<8;++i)CHECK(r8(NULL,c+0x189c+i)==((!present&&i==n)?(vals[k]&0xff):(0x40+i%3)));
        ++cases;}
    /* 1081e28c: slot state. */
    for(uint32_t idx=0;idx<12;++idx)for(unsigned m=0;m<16;++m){reset();present_mask=m;w32(NULL,CTX+4,OBJ);
        const uint32_t slot=(idx+2)&3;snprintf(want,sizeof want,"spa000/%u ",slot);
        CHECK(px_battle_slot_state(CTX,idx,&api)==((m>>slot&1)?slot:4));EXPECT(want);++cases;
        reset();present_mask=m;w32(NULL,CTX+4,OBJ);px_battle_slot_state(CTX,0xfffffffeu,&api);++cases;}
    /* 1082001c: the partner entry. */
    for(uint32_t o=0;o<6;++o)for(unsigned m=0;m<32;++m){reset();present_mask=m;owner_value=o;w32(NULL,CTX+4,OBJ);w32(NULL,CTX+8,0x8000);
        uint32_t slot=(o+2)&3;strcpy(want,"ow9 ");snprintf(want+4,sizeof want-4,"spa000/%u ",slot);
        if(!(m>>slot&1))slot=4;
        char t[40];snprintf(t,sizeof t,"spa000/%u ",slot);strcat(want,t);
        const uint32_t r=px_battle_partner_entry(CTX,9,&api);
        const uint32_t exp=(m>>slot&1)?0x8000u+28*slot+4:0u;
        CHECK(r==exp);EXPECT(want);CHECK(aborts==((m>>slot&1)&&slot>=4?1u:0u));++cases;}
    /* 1081cf14: the 0x37 record. */
    for(unsigned i=0;i<20;++i){reset();w32(NULL,CTX+0xc,0x7000+i);const uint32_t a=rnd(),b=rnd(),c=rnd(),d=rnd(),e=rnd();
        px_battle_record_0x37(CTX,a,b,c,d,e,&api);snprintf(want,sizeof want,"ev%x/37/%x/%x/%x/%x/%x ",0x7000+i,a,b,c,d,e);EXPECT(want);++cases;}
    /* 1081d2b0: entries with a better score and another battler. */
    for(unsigned t=0;t<400;++t){reset();entry_count=rnd()%6;uint32_t ids[8],mine=0x100;
        entry_ptrs[7]=0x500;w8(NULL,0x500+0x19,(uint8_t)(rnd()%3));score[7]=rnd()%5;
        for(unsigned i=0;i<entry_count;++i){entry_ptrs[i]=0x600+0x40*i;ids[i]=rnd()%3;w8(NULL,entry_ptrs[i]+0x19,(uint8_t)ids[i]);score[i]=rnd()%5;}
        (void)mine;
        const uint32_t arg=rnd();
        const uint32_t r=px_battle_count_better(CTX,0x500,arg,&api);
        uint32_t exp=0;for(unsigned i=0;i<entry_count;++i)if(ids[i]!=r8(NULL,0x500+0x19)&&score[i]>score[7])++exp;
        CHECK(r==exp);++cases;}
    /* 1081d770: the event 0xb check. */
    for(unsigned id=0;id<8;++id)for(unsigned ty=0;ty<3;++ty)for(unsigned hp=0;hp<3;++hp)for(unsigned al=0;al<2;++al){
        static const uint32_t types[3]={7,0,6},hps[3]={0,1,0x80000000u};
        reset();allow_value=al;hp_value=hps[hp];
        for(unsigned i=0;i<8;++i){w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x30+i));w8(NULL,MON+0x100*i+0x100,(uint8_t)types[ty]);w8(NULL,MON+0x100*i+0x101,0x12);}
        w8(NULL,0x7c00,0xb);w32(NULL,0x7c04,0);w32(NULL,HANDLER+8,0x7c00);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w32(NULL,HANDLER,0);w32(NULL,HANDLER+4,0);w32(NULL,BATTLE+0x60,HANDLER);
        const uint32_t r=px_battle_event_0xb_allows(CTX,id,&api);
        const int missing=id==7;
        uint32_t exp;char exps[200];exps[0]=0;
        if(!missing&&types[ty]==7)exp=1;
        else{snprintf(exps,sizeof exps,"v%x/d ",missing?0u:MON+0x100*id);
            if(hps[hp]==0)exp=0;
            else{const uint32_t idb=missing?0u:0x30+id;char t[64];snprintf(t,sizeof t,"h2=%x ",idb);strcat(exps,t);exp=al;}}
        CHECK(r==exp);
        { if(strcmp(log_,exps)){fprintf(stderr,"id %u ty %u hp %u al %u\ngot  %s\nwant %s\n",id,ty,hp,al,log_,exps);CHECK(0);}}
        if(aborts!=(missing?1u:0u)){fprintf(stderr,"aborts %u id %u ty %u hp %u al %u log %s\n",aborts,id,ty,hp,al,log_);}
        CHECK(aborts==(missing?1u:0u));++cases;}
    /* 108261e8: the chance event. */
    for(unsigned v=0;v<5;++v)for(unsigned ro=0;ro<6;++ro){static const uint32_t vals[5]={0,50,100,150,1},rolls[6]={0,49,50,99,100,0xffffffffu};
        reset();handler_keys=1;w32(NULL,RNG+0x54,0x1234abcd);roll_value=rolls[ro];w8(NULL,0x7c00,0xb5);w32(NULL,0x7c04,0);w32(NULL,HANDLER+8,0x7c00);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w32(NULL,HANDLER,0);w32(NULL,HANDLER+4,0);w32(NULL,BATTLE+0x60,HANDLER);
        const uint32_t r=px_battle_chance_event(CTX,0x11,0x22,vals[v],&api);
        snprintf(want,sizeof want,"h2=%x h3=11 h4=22 rb1234abcd/100 ",px_battle_event_get(0x02,&events)==0?0u:0u);
        CHECK(r==(rolls[ro]<vals[v]));
        CHECK(strstr(log_,"h3=11 h4=22 rb1234abcd/100 ")!=NULL);++cases;}
    /* 1082b064: set a move slot. */
    for(unsigned fi=0;fi<3;++fi)for(unsigned mv=0;mv<3;++mv)for(unsigned li=0;li<5;++li)for(unsigned pp=0;pp<4;++pp)for(unsigned cf=0;cf<2;++cf){
        static const uint32_t firsts[3]={1,0,0x100},moves[3]={0,0x1234,0x10001},limits[5]={0,5,0x100,0x200,4},pps[4]={0,5,0x1ff,0x37};
        reset();const uint32_t S=0xa000;uint8_t before[0x20];fill_noise();for(unsigned i=0;i<0x20;++i)w8(NULL,S+i,(uint8_t)(0x90+i));w8(NULL,S+0xc,(uint8_t)cf);
        pp_value=pps[pp];memcpy(before,mem+S,0x20);
        px_battle_set_move_slot(0xdead,S,moves[mv],limits[li],firsts[fi],&api);
        uint8_t exp[0x20];memcpy(exp,before,0x20);const unsigned fst=firsts[fi]!=0;const unsigned b=fst?0:6;
        if(fst)exp[5]=0;else exp[0xb]=0;
        exp[b]=(uint8_t)moves[mv];exp[b+1]=(uint8_t)(moves[mv]>>8);
        uint32_t cnt=moves[mv]==0?0:(pps[pp]&0xff);exp[b+3]=(uint8_t)cnt;
        if(limits[li]!=0&&cnt>limits[li])exp[b+3]=(uint8_t)limits[li];
        exp[b+2]=exp[b+3];if(!fst)exp[0xc]=0;
        CHECK(memcmp(mem+S,exp,0x20)==0);
        want[0]=0;if(moves[mv]!=0)snprintf(want,sizeof want,"pp%x/0 ",moves[mv]);
        if(fst&&cf)snprintf(want+strlen(want),sizeof want-strlen(want),"cs%x/%x ",S+6,S);
        EXPECT(want);++cases;}
    /* 1081fc20: clear a bit of the battler's array and mark its entry. */
    for(unsigned t=0;t<600;++t){reset();fill_noise();const uint32_t B=0xa000,C=0x2000;
        const uint32_t index=t<300?rnd()%40:rnd()%0x800;const uint32_t quiet=rnd()%2,flag84=(uint32_t[]){0,7,8,0x10,0xffffffffu,4}[rnd()%6];
        for(unsigned i=0;i<0x110;++i)w8(NULL,B+0x169+i,(uint8_t)rnd());
        if(rnd()%4)w8(NULL,B+0x169+(index>>3),(uint8_t)(r8(NULL,B+0x169+(index>>3))|(1u<<(index&7))));
        w32(NULL,B+0x84,flag84);w8(NULL,B+0x19,0x33);w32(NULL,C+0xc,0x7777);
        const uint32_t n=rnd()%5;w8(NULL,C+0x108c,(uint8_t)n);
        for(unsigned i=0;i<4;++i){const uint32_t bp=0xa800+0x40*i;w8(NULL,bp+0x19,(uint8_t)(rnd()%2?0x33:0x34));w32(NULL,C+0x10bc+16*i,bp);
            w32(NULL,C+0x10c0+16*i,(rnd()%3==0?8u:rnd()%16)|(rnd()&0xfffffff0u));w8(NULL,C+0x10bc+16*i+0xd,0);}
        uint8_t snapshot[0x4000-0x2000+0x2000];(void)snapshot;
        uint8_t before[SIZE];memcpy(before,mem,SIZE);
        const uint32_t r=px_battle_clear_bit_flag(C,B,index,quiet,&api);
        const int had=(before[B+0x169+(index>>3)]>>(index&7))&1;
        CHECK(r==(uint32_t)(had?1:0));
        uint8_t expmem[SIZE];memcpy(expmem,before,SIZE);
        want[0]=0;
        if(had){expmem[B+0x169+(index>>3&0xff)]&=(uint8_t)~(1u<<(index&7));
            snprintf(want,sizeof want,"ev7777/1c/33/%x/0/0/0 ",index);
            if(flag84&7){snprintf(want+strlen(want),sizeof want-strlen(want),"q%x/%x/1a/0 ",C,B);}
            strcat(want,"ev7777/38/33/0/0/0/0 ");
            if(!quiet)for(unsigned i=0;i<n;++i){const uint32_t kind=r32(NULL,C+0x10c0+16*i)&0xf;if(kind==8)continue;
                if(before[0xa800+0x40*i+0x19]!=0x33){continue;}
                expmem[C+0x10bc+16*i+0xd]=1;break;}}
        CHECK(memcmp(mem,expmem,SIZE)==0);EXPECT(want);++cases;}
    printf("battle_low_1: %lu cases, all tests passed\n",cases);
    return 0;
}
