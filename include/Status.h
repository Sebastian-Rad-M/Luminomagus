#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <functional>
// Forward declarations
class Card;
class RoundTracker;
class CardZone;

class IStatus {
protected:
    int charges;

public:
    IStatus(int startingCharges) : charges(startingCharges) {}
    virtual ~IStatus() = default;
    
    virtual std::string getName() const = 0;
    bool isExpired() const { return charges <= 0; }
    
    virtual void modifyCost(int& /*r*/, int& /*b*/, int& /*g*/, int& /*generic*/) {}
    virtual void onCardPlayed(Card& /*card*/, RoundTracker& /*state*/) {}
    virtual void modifyDestination(CardZone*& /*destination*/, RoundTracker& /*state*/) {}   
    virtual std::unique_ptr<IStatus> clone() const = 0;
};

class OverchargeStatus : public IStatus {
public:
    explicit OverchargeStatus(int c = 1) : IStatus(c) {}
    std::string getName() const override { return "Overcharged"; }

    std::unique_ptr<IStatus> clone() const override {
        return std::make_unique<OverchargeStatus>(charges);
    }

    // Implementation moved to .cpp
    void onCardPlayed(Card& card, RoundTracker& state) override;
};

class EchoStatus : public IStatus {
public:
    explicit EchoStatus(int c = 1) : IStatus(c) {}
    std::string getName() const override { return "Echo"; }
    
    std::unique_ptr<IStatus> clone() const override {
        return std::make_unique<EchoStatus>(charges);
    }
    
    void onCardPlayed(Card& card, RoundTracker& state) override;
};

class GlobalExileStatus : public IStatus {
public:
    explicit GlobalExileStatus(int c) : IStatus(c) {}
    std::string getName() const override { return "Dreaming of the past"; }
    
    std::unique_ptr<IStatus> clone() const override {
        return std::make_unique<GlobalExileStatus>(charges);
    }
    
    void modifyDestination(CardZone*& destination, RoundTracker& state) override;
};

class CostReductionStatus : public IStatus {
   public:
	explicit CostReductionStatus(int c) : IStatus(c) {}
	std::string getName() const override { return "Kineticism"; }
	std::unique_ptr<IStatus> clone() const override {
		return std::make_unique<CostReductionStatus>(charges);
	}
	void modifyCost(int& /*r*/, int& /*b*/, int& /*g*/, int& generic) override {
		if (generic > 0) generic -= 1;
	}
};

class LambdaStatus : public IStatus {
private:
    std::string name;
    std::function<void(Card&, RoundTracker&)> playAction;
public:
    LambdaStatus(std::string n, int c, std::function<void(Card&, RoundTracker&)> action) 
        : IStatus(c), name(std::move(n)), playAction(std::move(action)) {}
    
    std::string getName() const override { return name; }
    std::unique_ptr<IStatus> clone() const override { 
        return std::make_unique<LambdaStatus>(name, charges, playAction); 
    }
    void onCardPlayed(Card& card, RoundTracker& state) override {
        if (playAction) playAction(card, state);
    }
};