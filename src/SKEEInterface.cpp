#include "SKEE.h"
#include "MannequinInterface.h"

class NameVisitor : public SKEE::IBodyMorphInterface::MorphVisitor
{
public:

	void Visit(RE::TESObjectREFR* actor, const char* name) override
	{
		morphList.push_back(name);
	}

	std::vector<std::string> morphList;
};

class KeyVisitor : public SKEE::IBodyMorphInterface::MorphKeyVisitor
{
public:

	void Visit(const char* name, float value) override
	{
		morphKeyList.emplace(name, value);
	}

	std::map<std::string, float> morphKeyList;
};

class NodeScaleVisitor : public SKEE::INiTransformInterface::NodeVisitor
{
public:

	bool VisitPosition(const char* a_node, const char* a_key, SKEE::INiTransformInterface::Position& a_position) override
	{
		return false;
	}

	bool VisitRotation(const char* a_node, const char* a_key, SKEE::INiTransformInterface::Rotation& a_rotation) override
	{
		return false;
	}

	bool VisitScale(const char* a_node, const char* a_key, float a_scale) override
	{
		MannequinInterface::Node node { a_node, a_key, a_scale };
		MannequinInterface::GetSingleton()->playerNodes.push_back(node);

		return true;
	}

	bool VisitScaleMode(const char* a_node, const char* a_key, uint32_t a_scaleMode) override
	{
		auto* playerNodes = &MannequinInterface::GetSingleton()->playerNodes;

		if (playerNodes->empty())
			return false;

		for (int i = 0; i < playerNodes->size(); i++) {		
			if (playerNodes->at(i).node == a_node && playerNodes->at(i).key == a_key)
				playerNodes->at(i).scaleMode = a_scaleMode;
		}

		return true;
	}
};
