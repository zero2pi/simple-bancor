#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>
#include <string>
#include<math.h>

using namespace eosio;
using eosio::asset;
using std::string;

class converter : public eosio::contract {

private:
using contract::contract;

// @abi table args i64

struct convert {
            asset          supply;  //total supply of converter currency
            asset          balance; // current balance of converter currency
            

            uint64_t primary_key()const { return supply.symbol.name(); }
         };  
 

typedef eosio::multi_index<N(args),convert> con;

public:

    int64_t weight=500000;
    int64_t base=1000000;
    int64_t bal=0;
    int64_t supp=0;
    
/// @abi action 
    void addsupply(asset supply,asset balance)
    {
        con convert(_self,_self);
	    auto itr=convert.find(supply.symbol.name());
	    eosio_assert(itr==convert.end(), "Currency already exists");
	    print (itr==convert.end());
	    convert.emplace(_self, [&](auto& a){
	    a.supply=supply;
	    a.balance=balance;
	    });
         print("adding new currency");
    }

    void updatesupply(asset supply,asset balance)
    {
        con convert(_self,_self);
	    auto itr=convert.find(supply.symbol.name());
	   // eosio_assert(itr==convert.end(), "Currency already exists");
	    if((itr==convert.end()))
            print("Token doesn't exist");
        
        else{
            convert.modify(itr,0, [&](auto& a){
	        a.supply=supply;
	        a.balance=balance;
	    });
        }
    }

     auto toconverter(asset first)
    {
	    con convert(_self,_self);
	   auto itr=convert.cbegin();
	    auto itr1=convert.cend();
        for_each(itr,itr1,[&](auto& a){
             bal=bal+a.balance.amount;
             supp=supp+a.supply.amount;
        });
	   // auto bal=itr->balance;
       // auto supp=itr->supply;
        print ("bal---",bal, ",supp--", supp);
	    int64_t amt=first.amount+bal;
        print ("--amt--",amt);
        convert.modify(itr,0, [&](auto& a){
	    a.balance.amount=amt;
	    });


        auto previous_balance = amt - first.amount; 
        print ("--prev blc--" ,previous_balance);

        auto init_price = (float)(previous_balance * base)/(weight * supp);
         print ("--init_price--" ,init_price);

       auto init_out   = init_price * first.amount;
        print ("--init_out--" ,init_out);
        auto out_price  = (float)(amt * base) / (weight * (supp+init_out) );
        print ("--out_price--" ,out_price);
        auto final_out  = out_price * first.amount;
        print ("--final_out--" ,final_out);
        convert.modify(itr,0, [&](auto& a){
	    a.balance.amount+=final_out;
        a.supply.amount+=final_out;
    	});
   
   
        return final_out;
	
    }

    auto fromcon(int64_t final_out)
    {
        bal=0;
        supp=0;
        con convert(_self,_self);
	    auto itr=convert.cbegin();
	    auto itr1=convert.cend();
        for_each(itr,itr1,[&](auto& a){
             bal=bal+a.balance.amount;
             supp=supp+a.supply.amount;
        });
	  //  auto bal=itr->balance;
       // auto supp=itr->supply;
        print ("--after tocon blc--",bal, ",,", supp);
        auto init_price = (float)(bal*base) / (weight * supp);
        print ("--init_price--", init_price);
        print ("--final_out--", final_out);
        auto init_out   = init_price * final_out;
        print ("--init_out--", init_out);
        
        convert.modify(itr,0, [&](auto& a){
	    a.balance.amount-=final_out;
        a.supply.amount-=final_out;
    	});
        
        auto out_price  = (float)((bal-init_out)*base) / ( weight * (supp) );
        print ("--out_price--", out_price);
        auto finalout= out_price * final_out;
       print ("--final bskt token--", finalout);

        return finalout;
    }
     
    void onconvert(asset first) //asset sym is the symbol of converter currency
    {
        con convert(_self,_self);
        
       auto out=toconverter(first);
       print("--tocon o/p--" , out );
       auto finall=fromcon(out);
       int64_t f = (int64_t)round(finall);
       print ("---final bskt token---: ", f);
    }
    void hi()
    {
        print("hi");
    }



};

EOSIO_ABI( converter,(onconvert)(addsupply)(updatesupply)(hi))
