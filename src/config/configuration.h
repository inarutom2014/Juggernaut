#pragma once

namespace Jugg {
    class Config {
    public:
      static Config *getInstance();
      inline bool isBigEndian() const { return isBigEndian_; }

    private:
      static Config* instance_s;
      void init();
      void checkEndian();
    private:
      bool isBigEndian_;    //是否是大端

    };



}