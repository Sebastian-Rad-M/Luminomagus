#pragma once
#include <exception>
#include <string>

class GameException : public std::exception {
protected:
    std::string message;
public:
    explicit GameException(std::string msg) : message(std::move(msg)) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class ResourceException : public GameException {
public:
    using GameException::GameException;
};

class LogicException : public GameException {
public:
    using GameException::GameException;
};

class DatabaseException : public GameException {
public:
    using GameException::GameException;
};
