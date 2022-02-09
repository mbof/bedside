// vim: ts=2 sw=2 et

#ifndef MODEL_H
#define MODEL_H

class BedsideModel {
public:
  BedsideModel(){};
  char *getTime();

private:
  char time_str[6];
};

#endif
