#include <eosio/eosio.hpp>
#include "base58.hpp" 

#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>   
#include <eosio/crypto.hpp>        

#include "businesscard.hpp"     


 
/*
businesscard. (31.okt.2021 - Beginn)

So. 31.okt.2021  - Begin
Di. 02.nov.2021  - Final version
*/

using namespace eosio;
using namespace std;



const std::string   version   = "V1.0";
 

CONTRACT businesscard : public eosio::contract {

  public:
      using contract::contract;
      
      // ---
      // Constants
      //
      
      
      
      // ---
      // struct global_s
      //
      struct [[eosio::table]]  global_s
      {
      uint64_t id; 
      uint64_t mintnumber; 
      
      auto primary_key() const { return id; }
      EOSLIB_SERIALIZE(global_s, (id)(mintnumber) )
      }; // struct global      
      
      typedef eosio::multi_index< name("global"), global_s> globals_t;
      
     
    
      
        

      //Scope: collection_name
      TABLE schemas_s {
        name            schema_name;
        vector <FORMAT> format;

        uint64_t primary_key() const { return schema_name.value; }
      };

      typedef multi_index <name("schemas"), schemas_s> schemas_t;
    
    
    


    //Scope: owner
    TABLE assets_s {
        uint64_t         asset_id;
        name             collection_name;
        name             schema_name;
        int32_t          template_id;
        name             ram_payer;
        vector <asset>   backed_tokens;
        vector <uint8_t> immutable_serialized_data;
        vector <uint8_t> mutable_serialized_data;

        uint64_t primary_key() const { return asset_id; };
    };

    typedef multi_index <name("assets"), assets_s> assets_t;
    
    
      
            

      struct transfer_args
      {
      name from;
      name to;
      asset quantity;
      std::string memo;
      };    

      struct transfer_args_nft
      {
      name from;
      name to;
      vector <uint64_t> sender_asset_ids;
      std::string memo;
      };    
    
    
    
    
   assets_t  get_assets(name acc) 
     {
     return assets_t( name("atomicassets")  , acc.value); 
     }

    
   schemas_t  get_schemas(name collection_name) 
     {
     return schemas_t(  name("atomicassets") , collection_name.value);         
     }
           
      
      
        ATTRIBUTE_MAP deserialize(const std::vector <uint8_t> &data, const std::vector <FORMAT> &format_lines) {
        ATTRIBUTE_MAP attr_map = {};

        auto itr = data.begin();
        while (itr != data.end()) {
            uint64_t identifier = unsignedFromVarintBytes(itr);
            FORMAT format = format_lines.at(identifier - RESERVED);
            
            print(" T:(",format.type, "-" ,format.name, ") ");
            attr_map[format.name] = deserialize_attribute(format.type, itr);
        }

        return attr_map;
        }
    
    
      // ---
      // Status - minimal state
      [[eosio::action]]  
      void status() 
      {
      require_auth(get_self());
      
      print(" VERSION businesscard: ",version," ");
      } // status() 
    
    
    // Get current timestamp
    uint32_t now() {
    return (uint32_t)(eosio::current_time_point().sec_since_epoch());
    }

    
      // ---
      // Mint/create a new businesscard
      //
      [[eosio::action]]  
      void mintcard (
                    name   creatoracc,  // Wallet of businesscard-creator
                    string realname,    // Name of owner, like "Sven Pohl"
                    string imghash,     // IPFS-hash of the image
                    string info         // Initial contact information (e-mail, telegram, website...)               
                    ) 
      {
      require_auth( creatoracc );
  
      // Get last mintnumber from globals-table    
      globals_t myglobals( _self , _self.value );         
      auto giterator = myglobals.find(0);
      uint64_t mintnumber = giterator->mintnumber; 
      mintnumber++;
    
    
            
      // get current timestamp  
      auto thetime = now();
               
      int p_year;
      int p_month;
      int p_day;
      int p_hour;
      int p_minute; 
      int p_second;     
      
      convert_ts(  (unsigned long int) thetime,
                           &p_year, &p_month, &p_day,
                           &p_hour, &p_minute, &p_second);
 
      // Create date-string         
      std::string str_year    =  std::to_string(  p_year  );  
      std::string str_month   =  std::to_string(  p_month  );  
      std::string str_day     =  std::to_string(  p_day  );                            
      std::string str_hour    =  std::to_string(  p_hour  );                            
      std::string str_minute  =  std::to_string(  p_minute  );                            
      std::string memo_date   =  str_month + "/" + str_day + "/" + str_year + " " + str_hour + ":" + str_minute;
            
 
      // Create initial immutable and mutable data
      ATTRIBUTE_MAP imdata = {};
      ATTRIBUTE_MAP mdata = {};
      
      /*
      Fields:
      name
      img
      creator
      mintnumber
      info
      updated
      updatecnt
      */
      
 
      imdata["name"]       = realname;
      imdata["img"]        = imghash;
      imdata["creator"]    = creatoracc.to_string();
      imdata["mintnumber"] = mintnumber;

      mdata["info"]      = info;      
      mdata["updated"]   = memo_date;
      mdata["updatecnt"] = uint64_t(0); // How many times is it updated? Initial value is 0.
      
     
       
      uint32_t template_id = 340765; // Constant template for the businesscard
      
      action(
      permission_level{name("businesscard"), name("active")},
      name("atomicassets"),
      name("mintasset"),
      make_tuple(  
      name("businesscard"),  // authorized_minter name
      name("bu5ine55card"),  // collection_name ("with '5' instead of 's', someone reserved the collectionname early ")
      name("card"),          // schema_name
      template_id,           // template_id (constant 340765)
      creatoracc,            // the creator is the first businesscard-owner 
      imdata,                // immutable_data ATTRIBUTE_MAP
      mdata,                 //  mutable_data ATTRIBUTE_MAP
      (vector <asset>) {}    // tokens_to_back asset [] - empty
      )).send();
   
      // Update mintnumber
      if ( giterator != myglobals.end() )
            {                        
            myglobals.modify(giterator, _self, [&](auto& global) 
                  {                                                    
                  global.mintnumber= mintnumber;                                                            
                  });            
            } // if...
      
      print(" mintcard ");
      } // mintcard()
      
      
      
      
      
      // ---
      // Update/edit the minted businesscard
      //
      [[eosio::action]]  
      void editcard (
                    uint64_t assetid, // AssetID of the NFT-businesscard
                    name creatoracc,  // Original owner of the card  
                    name owneracc,    // Current owner-account [important!]          
                    string info       // New updated contact information (e-mail, telegram, website...)            
                    ) 
      {              
      require_auth( creatoracc ); 
       

      // Get assets of the owneraccount
      assets_t owner_assets = get_assets( owneracc );
 

  
      auto asset_itr = owner_assets.require_find( assetid,   "No asset with this id exists in this account");

           
      // print("FOUND: ", asset_itr->template_id , " " );    
      
      
      //     
      // Read current mutable data (deserialize) 
      // This step is a little more complex
      //
     
           
      
      schemas_t collection_schemas = get_schemas( name("bu5ine55card") );       
      name n_schema = name("card");
      auto schema_itr = collection_schemas.find( n_schema.value  );
           
      
        ATTRIBUTE_MAP deserialized_old_data = deserialize(
        asset_itr->mutable_serialized_data,
        schema_itr->format
        );
      
      
      
     
      // get current timestamp  
      auto thetime = now();
               
      int p_year;
      int p_month;
      int p_day;
      int p_hour;
      int p_minute; 
      int p_second;     
      
      convert_ts(  (unsigned long int) thetime,
                           &p_year, &p_month, &p_day,
                           &p_hour, &p_minute, &p_second);
 
      // Create date-string         
      std::string str_year    =  std::to_string(  p_year  );  
      std::string str_month   =  std::to_string(  p_month  );  
      std::string str_day     =  std::to_string(  p_day  );                            
      std::string str_hour    =  std::to_string(  p_hour  );                            
      std::string str_minute  =  std::to_string(  p_minute  );                            
      std::string memo_date   =  str_month + "/" + str_day + "/" + str_year + " " + str_hour + ":" + str_minute;
       
      
    
     // Get the number of past updates for increment
     uint64_t updatecnt =    std::get< uint64_t > (deserialized_old_data["updatecnt"]);  
     
      
    
     //
     // Save updated data
     //
     updatecnt++;

     // Set updated counter and new info (e-mail, contract information, website...)
     deserialized_old_data["updatecnt"] = updatecnt;
     deserialized_old_data["updated"]   = memo_date;     
     deserialized_old_data["info"]      = info;
     
            

      action(
       permission_level{name("businesscard"), name("active")},
       name("atomicassets"),
       name("setassetdata"),
       make_tuple(name("businesscard"), owneracc, assetid, deserialized_old_data))
      .send();
                 
      } // mintcard()
      
      
    
 


void convert_ts(unsigned long int unixtime,
                           int *p_year, int *p_month, int *p_day,
                           int *p_hour, int *p_minute, int *p_second)
{
    const unsigned long int SECONDS_PER_DAY    =  86400ul; 
    const unsigned long int DAYS_IN_YEAR       =    365ul; 
    const unsigned long int DAYS_IN_4_YEARS    =   1461ul; 
    const unsigned long int DAYS_IN_100_YEARS  =  36524ul; 
    const unsigned long int DAYS_IN_400_YEARS  = 146097ul; 
    const unsigned long int DAYN_AD_1970_01_01 = 719468ul; 

    unsigned long int DayN = DAYN_AD_1970_01_01 + unixtime/SECONDS_PER_DAY;
    unsigned long int seconds_since_midnight   = unixtime%SECONDS_PER_DAY;
    unsigned long int temp;

    temp = 4 * (DayN + DAYS_IN_4_YEARS + 1) / DAYS_IN_400_YEARS - 1;
    *p_year = 100 * temp;
    DayN -= DAYS_IN_100_YEARS * temp + temp / 4;
   
    temp = 4 * (DayN + DAYS_IN_YEAR + 1) / DAYS_IN_4_YEARS - 1;
    *p_year += temp;
    DayN -= DAYS_IN_YEAR * temp + temp / 4;

    *p_month = (5 * DayN + 2) / 153;
    *p_day = DayN - (*p_month * 153 + 2) / 5 + 1;
     
    *p_month += 3; 
    
    if (*p_month > 12)
    {       
        *p_month -= 12;
        ++*p_year;
    }

    *p_hour    = seconds_since_midnight / 3600;
    *p_minute  = seconds_since_midnight % 3600 / 60;
    *p_second  = seconds_since_midnight % 60;
} // convert_ts


 
    
 
     
  
       
      
      
      
}; // CONTRACT businesscard      




extern "C"
{

void apply(uint64_t receiver, uint64_t code, uint64_t action) 
{
 
 
    if (action == name("status").value)
       {
       execute_action(name(receiver), name(code), &businesscard::status);  
       }        
       
     if (action == name("mintcard").value)
       {
       execute_action(name(receiver), name(code), &businesscard::mintcard);  
       }        

     if (action == name("editcard").value)
       {
       execute_action(name(receiver), name(code), &businesscard::editcard);  
       }        
    

} // apply
    
    
    
       
} // extern "C"      

