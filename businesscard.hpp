/*

*/

 typedef std::vector <int8_t> INT8_VEC;
    typedef std::vector <int16_t> INT16_VEC;
    typedef std::vector <int32_t> INT32_VEC;
    typedef std::vector <int64_t> INT64_VEC;
    typedef std::vector <uint8_t> UINT8_VEC;
    typedef std::vector <uint16_t> UINT16_VEC;
    typedef std::vector <uint32_t> UINT32_VEC;
    typedef std::vector <uint64_t> UINT64_VEC;
    typedef std::vector <float> FLOAT_VEC;
    typedef std::vector <double> DOUBLE_VEC;
    typedef std::vector <std::string> STRING_VEC;

    typedef std::variant <\
        int8_t, int16_t, int32_t, int64_t, \
        uint8_t, uint16_t, uint32_t, uint64_t, \
        float, double, std::string, \
        INT8_VEC, INT16_VEC, INT32_VEC, INT64_VEC, \
        UINT8_VEC, UINT16_VEC, UINT32_VEC, UINT64_VEC, \
        FLOAT_VEC, DOUBLE_VEC, STRING_VEC> ATOMIC_ATTRIBUTE;

    typedef std::map <std::string, ATOMIC_ATTRIBUTE> ATTRIBUTE_MAP;
    
    
    
     struct FORMAT {
        std::string name;
        std::string type;
    };
    
    
      static constexpr uint64_t RESERVED = 4;
      
      
      
      
      

    uint64_t unsignedFromVarintBytes(std::vector <const uint8_t>::iterator &itr) {
        uint64_t number = 0;
        uint64_t multiplier = 1;

        while (*itr >= 128) {
            number += (((uint64_t) * itr) - 128) * multiplier;
            itr++;
            multiplier *= 128;
        }
        number += ((uint64_t) * itr) * multiplier;
        itr++;

        return number;
    }
          
          
          
          

    uint64_t unsignedFromIntBytes(std::vector <const uint8_t>::iterator &itr, uint64_t original_bytes = 8) {
        uint64_t number = 0;
        uint64_t multiplier = 1;

        for (uint64_t i = 0; i < original_bytes; i++) {
            number += ((uint64_t) * itr) * multiplier;
            multiplier *= 256;
            itr++;
        }

        return number;
    }          
          
          
          
  int64_t zigzagDecode(uint64_t value) {
        if (value % 2 == 0) {
            return (int64_t)(value / 2);
        } else {
            return (int64_t)(value / 2) * -1 - 1;
        }
    }          
          
          
          

    ATOMIC_ATTRIBUTE deserialize_attribute(const std::string &type, std::vector <const uint8_t>::iterator &itr) {
        if (type.find("[]", type.length() - 2) == type.length() - 2) {
            //Type is an array
            uint64_t array_length = unsignedFromVarintBytes(itr);
            std::string base_type = type.substr(0, type.length() - 2);

            if (type == "int8[]") {
                INT8_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int8_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "int16[]") {
                INT16_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int16_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "int32[]") {
                INT32_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int32_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "int64[]") {
                INT64_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int64_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "uint8[]" || type == "fixed8[]" || type == "bool[]") {
                UINT8_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint8_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "uint16[]" || type == "fixed16[]") {
                UINT16_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint16_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "uint32[]" || type == "fixed32[]") {
                UINT32_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint32_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "uint64[]" || type == "fixed64[]") {
                UINT64_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint64_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "float[]") {
                FLOAT_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <float>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "double[]") {
                DOUBLE_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <double>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "string[]" || type == "image[]") {
                STRING_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <std::string>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            }
        }

        if (type == "int8") {
            return (int8_t) zigzagDecode(unsignedFromVarintBytes(itr));
        } else if (type == "int16") {
            return (int16_t) zigzagDecode(unsignedFromVarintBytes(itr));
        } else if (type == "int32") {
            return (int32_t) zigzagDecode(unsignedFromVarintBytes(itr));
        } else if (type == "int64") {
            return (int64_t) zigzagDecode(unsignedFromVarintBytes(itr));

        } else if (type == "uint8") {
            return (uint8_t) unsignedFromVarintBytes(itr);
        } else if (type == "uint16") {
            return (uint16_t) unsignedFromVarintBytes(itr);
        } else if (type == "uint32") {
            return (uint32_t) unsignedFromVarintBytes(itr);
        } else if (type == "uint64") {
            return (uint64_t) unsignedFromVarintBytes(itr);

        } else if (type == "fixed8") {
            return (uint8_t) unsignedFromIntBytes(itr, 1);
        } else if (type == "fixed16") {
            return (uint16_t) unsignedFromIntBytes(itr, 2);
        } else if (type == "fixed32") {
            return (uint32_t) unsignedFromIntBytes(itr, 4);
        } else if (type == "fixed64") {
            return (uint64_t) unsignedFromIntBytes(itr, 8);

        } else if (type == "float") {
            uint8_t array_repr[4];
            for (uint8_t &i : array_repr) {
                i = *itr;
                itr++;
            }
            auto *val = reinterpret_cast<float *>(&array_repr);
            return *val;

        } else if (type == "double") {
            uint8_t array_repr[8];
            for (uint8_t &i : array_repr) {
                i = *itr;
                itr++;
            }
            auto *val = reinterpret_cast<double *>(&array_repr);
            return *val;

        } else if (type == "string" || type == "image") {
            uint64_t string_length = unsignedFromVarintBytes(itr);
            std::string text(itr, itr + string_length);

            itr += string_length;
            return text;

        } else if (type == "ipfs") {
            uint64_t array_length = unsignedFromVarintBytes(itr);
            std::vector <uint8_t> byte_array = {};
            byte_array.insert(byte_array.begin(), itr, itr + array_length);

            itr += array_length;
            return EncodeBase58(byte_array);

        } else if (type == "bool" || type == "byte") {
            uint8_t next_byte = *itr;
            itr++;
            return next_byte;

        } else {
            eosio::check(false, "No type could be matched - " + type);
            return ""; //This point can never be reached because the check above will always throw.
            //Just to silence the compiler warning
        }
    }
          
          
          
          
          
          
          
          
      
      
