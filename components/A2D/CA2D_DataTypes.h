// #included within CA2D class definition
// class CA2D {
//  public:
//   enum ModeType { UNSET, CONTINUOUS, TRIGGERED };'
    
    struct DataType {
      static const unsigned int NUM_CHANNELS = 8;
      static const unsigned int CHANNELS_BYTESIZE = NUM_CHANNELS * sizeof(int);

      CHead::StateType  State;
      uint32_t          timeStamp;
      uint32_t          timeDelta;
      int               Channels[NUM_CHANNELS];
      int               SerialCount;

      DataType(CHead::StateType state) : State(state), timeStamp(micros()), timeDelta(0) { memset(&Channels[0], 0, CHANNELS_BYTESIZE ); }
      DataType() : State(CHead::UNSET), timeStamp(micros()), timeDelta(0)                { memset(&Channels[0], 0, CHANNELS_BYTESIZE ); }
    };

    struct BlockType {
      static const unsigned int MAX_BLOCKSIZE = 4096;

      uint32_t               timeStamp;
      CHead::StateType       State;
      std::vector<DataType> *data;

      BlockType() : data(new std::vector<DataType>()) { data->reserve(MAX_BLOCKSIZE); }

      void writeSerial() volatile {
        if (data == NULL) return;
        
        Serial.write((uint8_t*)&timeStamp, sizeof(timeStamp));
        Serial.write((uint8_t*)&State, sizeof(State));

        uint32_t sampleCount = data->size();
        Serial.write((uint8_t*)&sampleCount, sizeof(sampleCount));

        if (data->empty() == false) {
          uint8_t* pVectorData = reinterpret_cast<uint8_t*>(data->data());
          size_t totalDataSize = data->size() * sizeof(DataType);
          Serial.write(pVectorData, totalDataSize);
        }

      }

    };
    
    typedef void (*CallbackType)(BlockType*);
