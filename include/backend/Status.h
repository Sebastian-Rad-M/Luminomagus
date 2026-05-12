#pragma once
#include <memory>
#include <string>
#include <functional>

class Card;
class RoundTracker;
class CardZone;

class IStatus {
protected:
    int charges=1;
    std::string name="";
    std::string description="";
    bool m_isBoss = false;

public:
    IStatus():charges(999) {}
    IStatus(int startingCharges) : charges(startingCharges) {}
    IStatus(std::string N) : name(N) {}
    virtual ~IStatus() = default;  
    std::string getName() {return name;};
    std::string getDescription() {return description;} 
    void setDescription(const std::string& desc) { description = desc; }
    bool isExpired() const { return charges <= 0; }
    bool isBoss() const { return m_isBoss; }
    void setIsBoss(bool boss) { m_isBoss = boss; }


    virtual void modifyCost(int& /*r*/, int& /*b*/, int& /*g*/, int& /*generic*/, RoundTracker& /*state*/) {};
    virtual void onCardPlayed(Card& /*card*/, RoundTracker& /*state*/) {}
    virtual void modifyDestination(CardZone*& /*destination*/, RoundTracker& /*state*/) {}   
    virtual bool canPlayCard(Card& /*card*/, RoundTracker& /*state*/) { return true; }
    virtual void modifyTargetScore(int& /*targetScore*/, RoundTracker& /*state*/) {}
    virtual void onApplied(RoundTracker& /*state*/) {}
    virtual void onCardDrawn(Card& /*card*/, RoundTracker& /*state*/) {}
    virtual void onManaAdded(int /*r*/, int /*b*/, int /*g*/, RoundTracker& /*state*/) {}
    virtual std::unique_ptr<IStatus> clone() const = 0;
};

class OverchargeStatus : public IStatus {
public:
    /// @brief copy the next spells once
    /// @param c c is the number of next spells
    explicit OverchargeStatus(int c = 1) : IStatus(c) {}
    std::unique_ptr<IStatus> clone() const override {return std::make_unique<OverchargeStatus>(charges);}
    void onCardPlayed(Card& card, RoundTracker& state) override;
};

class EchoStatus : public IStatus {
public:
    explicit EchoStatus(int c = 1) : IStatus(c) {}
    std::unique_ptr<IStatus> clone() const override {return std::make_unique<EchoStatus>(charges);}
    void onCardPlayed(Card& card, RoundTracker& state) override;
};

class GlobalExileStatus : public IStatus {
public:
    explicit GlobalExileStatus() : IStatus() {}
    std::unique_ptr<IStatus> clone() const override {return std::make_unique<GlobalExileStatus>();}
    void modifyDestination(CardZone*& destination, RoundTracker& state) override;
};

class CostReductionStatus : public IStatus {
   public:
	explicit CostReductionStatus(int c) : IStatus(c) {}
	std::unique_ptr<IStatus> clone() const override {return std::make_unique<CostReductionStatus>(charges);}
	void modifyCost(int& /*r*/, int& /*b*/, int& /*g*/, int& generic, RoundTracker& /*state*/) override {if (generic > 0) generic -= 1;}
};

class LambdaStatus : public IStatus {
   private:
	//std::string name;
	std::function<void(Card&, RoundTracker&)> playAction;
	std::function<void(int&, int&, int&, int&, RoundTracker&)> costAction;
	std::function<void(CardZone*&, RoundTracker&)> destAction;
	std::function<bool(Card&, RoundTracker&)> canPlayAction;
	std::function<void(int&, RoundTracker&)> scoreAction;
	std::function<void(RoundTracker&)> appliedAction;
    std::function<void(Card&, RoundTracker&)> drawAction;
    std::function<void(int, int, int, RoundTracker&)> manaAction;

   public:
	LambdaStatus(std::string n, int c) : IStatus(c) { this->name = std::move(n); }
    LambdaStatus(std::string n) : IStatus() { this->name = std::move(n); }
	
	std::unique_ptr<IStatus> clone() const override { 
		auto clone = std::make_unique<LambdaStatus>(this->name, charges);
        clone->setPlayAction(playAction).setCostAction(costAction).setDestAction(destAction).setCanPlayAction(canPlayAction).setScoreAction(scoreAction).setAppliedAction(appliedAction).setDrawAction(drawAction).setManaAction(manaAction); 
        return clone;
	}

	// Chainable Setters
	LambdaStatus& setPlayAction(std::function<void(Card&, RoundTracker&)> action) { playAction = std::move(action); return *this; }
	LambdaStatus& setCostAction(std::function<void(int&, int&, int&, int&, RoundTracker&)> action) { costAction = std::move(action); return *this; }
	LambdaStatus& setDestAction(std::function<void(CardZone*&, RoundTracker&)> action) { destAction = std::move(action); return *this; }
	LambdaStatus& setCanPlayAction(std::function<bool(Card&, RoundTracker&)> action) { canPlayAction = std::move(action); return *this; }
	LambdaStatus& setScoreAction(std::function<void(int&, RoundTracker&)> action) { scoreAction = std::move(action); return *this; }
	LambdaStatus& setAppliedAction(std::function<void(RoundTracker&)> action) { appliedAction = std::move(action); return *this; }
    LambdaStatus& setDrawAction(std::function<void(Card&, RoundTracker&)> action) { drawAction = std::move(action); return *this; }
    LambdaStatus& setManaAction(std::function<void(int, int, int, RoundTracker&)> action) { manaAction = std::move(action); return *this; }

	// Interface Implementations
	void onCardPlayed(Card& card, RoundTracker& state) override { if (playAction) playAction(card, state); }
	void modifyCost(int& r, int& b, int& g, int& generic, RoundTracker& state) override { if (costAction) costAction(r, b, g, generic, state); }
	void modifyDestination(CardZone*& destination, RoundTracker& state) override { if (destAction) destAction(destination, state); }
	bool canPlayCard(Card& card, RoundTracker& state) override { return canPlayAction ? canPlayAction(card, state) : true; }
	void modifyTargetScore(int& targetScore, RoundTracker& state) override { if (scoreAction) scoreAction(targetScore, state); }
	void onApplied(RoundTracker& state) override { if (appliedAction) appliedAction(state); }
    void onCardDrawn(Card& card, RoundTracker& state) override { if (drawAction) drawAction(card, state); }
    void onManaAdded(int r, int b, int g, RoundTracker& state) override { if (manaAction) manaAction(r, b, g, state); }
};