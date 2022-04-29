# Application Manager

The Application Manager service is responsible for application lifecycle and switching between applications and views.
This includes:
- starting
- closing
- registering
- switching applications and views
- managing display access

## Initialisation

![](./Images/appmgr_init.svg)

## Starting application

![](./Images/appmgr_start_app.svg)

##  Switching between applications

![](./Images/appmgr_switch_app.svg)

## Switching to previous application

![](./Images/appmgr_switch_back.svg)

## New Application Manager - Proposal

Main problems of current implementation:
1. Application knows the name of another application it wants to switch to.
2. Each application is responsible for implementing the logic of an action (e.g. sending an SMS) on its own. In that context, the Application Manager is responsible only for switching between apps.

The proposal of New Application Manager solves these problems.

![](./Images/appmgr_switch_sms_app_proposal.svg)

Applications do not request for application switch explicitly. Instead, applications send action requests to the Application Manager.
The action request may contain:
- Type of action, e.g. `SendSMS`
- Generic data storage

Depending on the action's type, the Application Manager decides which application should handle it, based on the application's manifest.
Thanks to that, Application Manager decouples applications.

### Registering applications

Before start, each application will register its manifest to the Application Manager.
The manifest will be stored in the Applications Registry.

The manifest may contain the following information:
- Application name
- Whether the application should run in background/foreground
- Actions it is able to handle

Once the action is requested, the Application Manager retrieves action handlers from the Application Registry.

### Generic data

It should be possible to send any custom data along with the action request.
This is the reason, why action request will implement storage for generic data.

The idea is to create a base for such generic data. It should be able to serialize and deserialize into/from a data storage, e.g. JSON, FlatBuffers, binary formats.

When sent, the data will be serialized into the data storage, and only that storage shall be sent along with the action request.

When received, the data will be re-created from the storage and used by a receiver object.

#### Interface proposal and code example

```
#include <string>
#include <cassert>

using DataPackage = std::string; // To be changed to chosen data format.

class Packageable {
public:
  virtual ~Packageable() noexcept = default;

  virtual DataPackage pack() const = 0;

  template <typename T> class Creator {
  public:
    virtual ~Creator() noexcept = default;

    virtual T unpack(const DataPackage &package) = 0;
  };
};

class ExampleData : public Packageable {
public:
  class ExampleCreator : public Packageable::Creator<ExampleData> {
  public:
    ExampleData unpack(const DataPackage &package) override {
      const auto x = std::stoi(package);
      return ExampleData{x};
    }
  };
  static inline ExampleCreator CREATOR;

  explicit ExampleData(int _x) : x{_x} {}

  DataPackage pack() const override { return std::to_string(x); }

  int getX() const noexcept { return x; }

private:
  int x{0};
};

int main() {
  ExampleData data{10};
  const auto package = data.pack();
  const auto received = ExampleData::CREATOR.unpack(package);
  assert(data.getX() == received.getX());
  return 0;
}
```
