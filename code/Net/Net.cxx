# include <Net.h>

Net::Net()
{
  NetSetId(0);
}

Net::Net(int id)
{
  NetSetId(id);
}

Net::Net(int id, const string& Name)
{
  NetSetId(id);
  NetSetName(Name);
}

void 
Net::NetSetName(const string& Name)
{
  name = Name;
}

void 
Net::NetSetId(int id)
{
  Id = id;
}

void
Net::NetAddPin(const Pin& pinToAdd)
{
  string Name = pinToAdd.PinGetName();

  Pins[Name] = (Pin *) &pinToAdd;
}

int
Net::NetGetId(void)
{
  return Id;
}

string
Net::NetGetName(void)
{
  return name;
}

map<string, Pin*>& Net::NetGetPins(void)
{
  return this->Pins;
}
