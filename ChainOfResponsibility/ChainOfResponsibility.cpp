
#include <iostream>
#include <fstream>

class LogMessage {
public:
   enum Type {
      Warning,
      Error,
      FatalError,
      UnknowMess
   };

   virtual Type type() const = 0;
   virtual const std::string& message() const = 0;
};

class Handler {
public:
   Handler(std::unique_ptr<Handler> next) : next_(std::move(next)) {}
   virtual ~Handler() = default;

   void handleMes(const LogMessage& message) {
      if (handlePush(message)) {
         return;
      }
      if (!next_) {
         throw std::runtime_error("Error: No handler");
      }
      next_->handleMes(message);
   }

private:
   virtual bool handlePush(const LogMessage& message) = 0;
   std::unique_ptr<Handler> next_;
};
 

class WarningHandler : public Handler {
public:
   using Handler::Handler;

private:
   bool handlePush(const LogMessage& mess) override {
      if (mess.type() != LogMessage::Type::Warning) {
         return false;
      }
      std::cout << "Warning: " << mess.message() << std::endl;
      return true;
   }
};

class ErrorHandler : public Handler {
public:
   ErrorHandler(std::unique_ptr<Handler> next) : Handler(std::move(next)) {
      file.open("ErrorMessage.txt", std::ios::app);
      if (!file.is_open()) {
         throw std::runtime_error("Error: Could not open error log file.");
      }
   }

   ~ErrorHandler() {
      if (file.is_open()) {
         file.close();
      }
   }

private:
   std::ofstream file;
   bool handlePush(const LogMessage& mess) override {
      if (mess.type() != LogMessage::Type::Error) {
         return false;
      }
      file << "Error: " << mess.message() << std::endl;
      return true;
   }
};

class FatalHandler : public Handler {
public:
   using Handler::Handler;

private:
   bool handlePush(const LogMessage& mess) override {
      if (mess.type() != LogMessage::Type::FatalError) {
         return false;
      }
      throw std::runtime_error("Fatal Error: " + mess.message());
   }
};

class UnknowHandler : public Handler {
public:
   using Handler::Handler;

private:
   bool handlePush(const LogMessage& mess) override {
      if (mess.type() != LogMessage::Type::UnknowMess) {
         return false;
      }
      throw std::runtime_error("Unknown Error: " + mess.message());
   }
};

class WarningMessage : public LogMessage {
public:
   Type type() const override { return Type::Warning; }
   const std::string& message() const override {
      static const std::string mess{ "An error may occur" };
      return mess;
   }
};

class ErrorMessage : public LogMessage {
public:
   Type type() const override { return Type::Error; }
   const std::string& message() const override {
      static const std::string msg = "Error happened";
      return msg;
   }
};

class FatalMessage : public LogMessage {
public:
   Type type() const override { return Type::FatalError; }
   const std::string& message() const override {
      static const std::string msg = "Fatal error happened";
      return msg;
   }
};

class UnknowMessage : public LogMessage {
public:
   Type type() const override { return Type::UnknowMess; }
   const std::string& message() const override {
      static const std::string msg = "Unknown error happened";
      return msg;
   }
};

int main() {
  

      auto fatal_handler = std::make_unique<FatalHandler>(nullptr);
      auto error_handler = std::make_unique<ErrorHandler>(std::move(fatal_handler));
      auto warning_handler = std::make_unique<WarningHandler>(std::move(error_handler));
      auto unknow_handler = std::make_unique<UnknowHandler>(std::move(warning_handler));

   try {
      unknow_handler->handleMes(WarningMessage());
      unknow_handler->handleMes(ErrorMessage());
      unknow_handler->handleMes(FatalMessage());
     
   }
   catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
   }

   try
   {
      unknow_handler->handleMes(UnknowMessage());
   }

   catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
   }

   return 0;
}
