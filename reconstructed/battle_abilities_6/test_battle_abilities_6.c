#include "battle_abilities_6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7400, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t status_mask,kind_value,chance_value,field_b,stat_count,sign_mask;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t hp_value,max_value;
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xd||k==0xe);return k==0xd?hp_value:max_value; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%x ",mon(p),s);CHECK(s<32);return status_mask>>s&1; }
static uint32_t move_kind(void *u,uint32_t m) { (void)u;notef("kind%x ",m,0);return kind_value; }
static uint32_t move_value(void *u,uint32_t m,uint32_t f) { (void)u;notef("mv%x/%x ",m,f);return f==0xa?chance_value:field_b; }
static uint32_t stat_change_count(void *u,uint32_t m) { (void)u;notef("scc%x ",m,0);return stat_count; }
static void stat_change(void *u,uint32_t m,uint32_t i,uint32_t out) { (void)u;(void)m;CHECK(out==SCRATCH);notef("sc%u ",i,0);static const uint32_t vals[4]={2,0xffffffffu,0,1};w32(NULL,out,vals[sign_mask>>(2*i)&3]); }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_6_api api={&base,has_status,move_kind,move_value,stat_change_count,stat_change,popup,action_record,commit,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_6_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_limber_cure,px_battle_oblivious_cure};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    status_mask=kind_value=chance_value=field_b=stat_count=sign_mask=hp_value=max_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
struct keys { uint32_t k2,k3,k4,k5,targets[4],k0c,k1e,move,k26,k33; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x05,k.k5,&events);for(unsigned i=0;i<4;++i)px_battle_event_set_id(0x06+i,k.targets[i],&events);
    px_battle_event_set_value(0x0c,k.k0c,&events);px_battle_event_set_value(0x1e,k.k1e,&events);px_battle_event_set_value(0x12,k.move,&events);
    px_battle_event_set_value(0x26,k.k26,&events);px_battle_event_set_value(0x33,k.k33,&events);
    static const uint32_t flags[4]={0x42,0x48,0x49,0x27};for(unsigned i=0;i<4;++i)px_battle_event_set_value(flags[i],0x700+i,&events);
    log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
/* Keys 0x42, 0x48, 0x49, 0x27 as expected: `set` lists the ones rewritten (index into the four). */
static void flags_are(uint32_t v42,uint32_t v48,uint32_t v49,uint32_t v27)
{
    CHECK(get(0x42)==v42&&get(0x48)==v48&&get(0x49)==v49&&get(0x27)==v27);
}
/* The expected log of 1084a890 and its answer. */
static uint32_t secondary_model(char *w,size_t n,uint32_t move)
{
    cat(w,n,"kind%x mv%x/a ",move,move);
    if(chance_value)return 1;
    if(kind_value==4){cat(w,n,"mv%x/b ",move,0);return (field_b&0xffff)!=8;}
    if(kind_value==6)return 1;
    if(kind_value==7){cat(w,n,"scc%x ",move,0);for(uint32_t i=0;i<stat_count;++i){cat(w,n,"sc%u ",i,0);if((sign_mask>>(2*i)&3)==1)return 0;}return 1;}
    return move==0x122;
}
int main(void)
{
    unsigned cases=0;char want[1024];
    const struct keys BASE={ME,OTHER,OTHER,0,{0,0,0,0},6,7,0x10021,0,0x777};
    /* The cures on event 0x02: each status, with and without it, inside the popup. */
    {
        static const fn cures[6]={px_battle_insomnia_cure,px_battle_limber_cure,px_battle_immunity_cure,px_battle_own_tempo_cure,
            px_battle_water_veil_cure,px_battle_magma_armor_cure};
        static const uint32_t statuses[6]={2,1,5,6,4,3};
        for(unsigned h=0;h<6;++h)for(uint32_t mask=0;mask<0x1000;mask+=0x55) {
            reset();status_mask=mask|(mask&1?1u<<statuses[h]:0);run(cures[h],BASE);
            const int on=status_mask>>statuses[h]&1;snprintf(want,sizeof want,"hs2/%x ",statuses[h]);
            if(on)strcat(want,"pop1/2 reca/2 commit pop2/2 ");
            EXPECT(want);
            if(on)CHECK(commits[0][5]==ME&&commits[0][4]==statuses[h]&&commits[0][0x11]==1&&commits[0][0x10]==0xa5&&r32(NULL,0)==0&&
                        (uint32_t)(commits[0][0]|commits[0][1]<<8|commits[0][2]<<16|(uint32_t)commits[0][3]<<24)==0x11);
            ++cases;
        }
        for(uint32_t mask=0;mask<4;++mask) {
            reset();status_mask=(mask&1?1u<<7:0)|(mask&2?1u<<0xb:0);run(px_battle_oblivious_cure,BASE);
            strcpy(want,"hs2/7 ");if(mask&1)strcat(want,"pop1/2 reca/2 commit pop2/2 ");strcat(want,"hs2/b ");if(mask&2)strcat(want,"pop1/2 reca/2 commit pop2/2 ");
            EXPECT(want);CHECK(ncommit==(unsigned)((mask&1)+(mask>>1)));if(mask==3)CHECK(commits[0][4]==7&&commits[1][4]==0xb);++cases;
        }
        for(unsigned bits=0;bits<6;++bits) {
            static const uint32_t k0c[3]={7,6,0x107};
            reset();struct keys k=BASE;k.k2=bits&1?ME:OTHER;k.k0c=k0c[bits>>1];w32(NULL,WORK,9);run(px_battle_truant_sleep,k);
            CHECK(log_[0]==0&&r32(NULL,WORK)==(k.k2==ME&&k.k0c==7?0u:9u));++cases;
        }
    }
    /* Shield Dust's single-target handlers. */
    for(unsigned who=0;who<8;++who)for(unsigned e=0;e<3;++e) {
        static const uint32_t k1e[3]={8,7,0x108};
        struct keys k=BASE;k.k2=who&1?ME:OTHER;k.k3=who&2?ME:OTHER;k.k4=who&4?ME:OTHER;k.k1e=k1e[e];
        const int t=k.k4==ME,s=t&&k.k3!=ME;
        reset();run(px_battle_shield_dust_effect,k);CHECK(log_[0]==0);flags_are(s?1:0x700,0x701,0x702,0x703);
        reset();run(px_battle_shield_dust_status,k);CHECK(log_[0]==0);flags_are(s&&k.k1e!=8?1:0x700,0x701,0x702,0x703);
        reset();run(px_battle_shield_dust_poison,k);CHECK(log_[0]==0);flags_are(0x700,t?1:0x701,0x702,0x703);
        reset();run(px_battle_shield_dust_flinch,k);CHECK(log_[0]==0);flags_are(t?1:0x700,0x701,0x702,0x703);
        reset();run(px_battle_sheer_force_flinch,k);CHECK(log_[0]==0);flags_are(k.k3==ME?1:0x700,0x701,0x702,0x703);
        cases+=5;
    }
    /* Shield Dust over the target list: only as the single target. */
    for(uint32_t n=0;n<5;++n)for(unsigned mask=0;mask<16;++mask) {
        reset();struct keys k=BASE;k.k5=n;for(unsigned i=0;i<4;++i)k.targets[i]=mask>>i&1?ME:(i==3?0x102:6);run(px_battle_shield_dust_targets,k);
        int found=0;for(unsigned i=0;i<n&&i<4;++i)if(k.targets[i]==ME){found=1;break;}
        CHECK(log_[0]==0);flags_are(0x700,found&&n==1?1:0x701,0x702,0x703);++cases;
    }
    /* Sheer Force over the secondary-effect test. */
    {
        static const uint32_t kinds[6]={0,4,6,7,1,0x104},fields[3]={8,7,0x10008},moves[3]={0x10021,0x10122,0x122};
        for(unsigned who=0;who<2;++who)for(unsigned ki=0;ki<6;++ki)for(uint32_t ch=0;ch<2;++ch)for(unsigned fi=0;fi<3;++fi)
        for(uint32_t sc=0;sc<3;++sc)for(uint32_t sm=0;sm<16;++sm)for(unsigned mi=0;mi<3;++mi) {
            struct keys k=BASE;k.k3=who?ME:OTHER;k.move=moves[mi];
            for(unsigned h=0;h<2;++h) {
                reset();kind_value=kinds[ki];chance_value=ch;field_b=fields[fi];stat_count=sc;sign_mask=sm;
                run(h?px_battle_sheer_force_effect:px_battle_sheer_force_power,k);
                want[0]=0;uint32_t on=0;if(who)on=secondary_model(want,sizeof want,k.move&0xffff);
                EXPECT(want);flags_are(h&&on?1:0x700,0x701,!h&&on?1:0x702,0x703);++cases;
            }
            reset();kind_value=kinds[ki];chance_value=ch;field_b=fields[fi];stat_count=sc;sign_mask=sm;
            want[0]=0;const uint32_t expect=secondary_model(want,sizeof want,k.move&0xffff);
            log_[0]=0;CHECK(px_battle_secondary_effect(k.move&0xffff,&api)==expect);EXPECT(want);++cases;
        }
    }
    /* Stench: a 10% flinch chance unless one is set. */
    for(unsigned bits=0;bits<8;++bits) {
        static const uint32_t k26[4]={0,1,0x100,0x101};
        reset();struct keys k=BASE;k.k3=bits&1?ME:OTHER;k.k26=k26[bits>>1];run(px_battle_stench,k);
        CHECK(log_[0]==0);flags_are(0x700,0x701,0x702,k.k3==ME&&(k.k26&0xff)==0?0xa:0x703);++cases;
    }
    /* Helpers: full HP and the signed maximum. */
    for(uint32_t hp=0;hp<4;++hp)for(uint32_t mx=0;mx<4;++mx){reset();hp_value=hp;max_value=mx;CHECK(px_battle_full_hp(MON+0x200,&api)==(hp==mx));++cases;}
    {static const int32_t vs[6]={-5,-1,0,1,2,0x7fffffff};for(unsigned i=0;i<6;++i)for(unsigned j=0;j<6;++j){CHECK(px_battle_max(vs[i],vs[j])==(vs[i]<vs[j]?vs[j]:vs[i]));++cases;}}
    /* Natural Cure on switch-out. */
    for(unsigned who=0;who<2;++who){reset();struct keys k=BASE;k.k2=who?ME:OTHER;run(px_battle_natural_cure,k);EXPECT(who?"reca/2 commit ":"");
        if(who){CHECK(commits[0][4]==0x26&&commits[0][5]==ME&&commits[0][0x11]==1&&commits[0][0x12]==1&&commits[0][0x13]==0xa5&&commits[0][0]==0x11&&commits[0][2]==0);}
        ++cases;}
    /* Regenerator: a third of max HP, at most what is missing, unless fainted or full. */
    for(unsigned who=0;who<2;++who)for(uint32_t hp=0;hp<12;++hp)for(uint32_t mx=0;mx<12;++mx) {
        reset();struct keys k=BASE;k.k2=who?ME:OTHER;hp_value=hp;max_value=mx;run(px_battle_regenerator,k);want[0]=0;
        if(who){strcat(want,"v2/d ");if(hp){strcat(want,"v2/d v2/e ");if(hp!=mx){const uint32_t third=mx/3?mx/3:1,miss=mx-hp,heal=third>miss?miss:third;
            strcat(want,"v2/e v2/e v2/d rec7/2 commit ");CHECK(commits[0][7]==ME&&commits[0][4]==1&&commits[0][5]==1&&commits[0][6]==0xa5&&
                (uint32_t)(commits[0][0x14]|commits[0][0x15]<<8|commits[0][0x16]<<16|(uint32_t)commits[0][0x17]<<24)==heal);}}}
        EXPECT(want);++cases;
    }
    /* Poison Heal: poison damage cancelled, 1/8 healed. Heatproof: burn damage halved toward zero, at least 1. */
    for(unsigned who=0;who<2;++who)for(unsigned st=0;st<5;++st)for(unsigned d=0;d<6;++d) {
        static const uint32_t statuses[5]={5,4,0x10005,0x10004,1},damage[6]={0,1,2,7,0xfffffffbu,0xffffffffu};
        reset();struct keys k=BASE;k.k2=who?ME:OTHER;k.k1e=statuses[st];k.k33=damage[d];max_value=40+d;run(px_battle_poison_heal,k);
        const int poison=who&&(k.k1e&0xffff)==5,burn=who&&(k.k1e&0xffff)==4;
        EXPECT(poison?"rec4/2 v2/e commit ":"");CHECK(get(0x33)==(poison?0u:k.k33));
        if(poison)CHECK(commits[0][6]==ME&&(uint32_t)(commits[0][4]|commits[0][5]<<8)==(40+d)/8&&commits[0][0]==0x11&&commits[0][2]==0x80);
        reset();max_value=40;run(px_battle_heatproof_burn,k);CHECK(log_[0]==0);
        const int32_t half=(int32_t)k.k33/2;CHECK(get(0x33)==(burn?(uint32_t)(half<1?1:half):k.k33));cases+=2;
    }
    /* Through the dispatcher: Limber then Oblivious on event 0x02. */
    {
        reset();status_mask=1u<<1|1u<<0xb;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x02);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(BASE);px_battle_event_dispatch(CTX,0x02,&events);
        EXPECT("hs2/1 pop1/2 reca/2 commit pop2/2 hs2/7 hs2/b pop1/2 reca/2 commit pop2/2 ");CHECK(aborts==0);++cases;
    }
    printf("battle_abilities_6: %u cases, all tests passed\n",cases);
    return 0;
}
