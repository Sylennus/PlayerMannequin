#pragma once

namespace SKEE
{
	class IPluginInterface
	{
	public:
		IPluginInterface(){};
		virtual ~IPluginInterface(){};

		virtual uint32_t GetVersion() = 0;
		virtual void     Revert() = 0;
	};

	class IInterfaceMap
	{
	public:
		virtual IPluginInterface* QueryInterface(const char* name) = 0;
		virtual bool              AddInterface(const char* name, IPluginInterface* pluginInterface) = 0;
		virtual IPluginInterface* RemoveInterface(const char* name) = 0;
	};

	struct InterfaceExchangeMessage
	{
		enum : uint32_t
		{
			kExchangeInterface = 0x9E3779B9
		};

		IInterfaceMap* interfaceMap = nullptr;
	};

	class IAddonAttachmentInterface
	{
	public:
		virtual void OnAttach(RE::TESObjectREFR* a_refr, RE::TESObjectARMO* a_armor, RE::TESObjectARMA* a_addon, RE::NiAVObject* a_object, bool a_firstPerson, RE::NiNode* a_skeleton, RE::NiNode* a_root) = 0;
	};

	class IBodyMorphInterface : public IPluginInterface
	{
	public:
		class MorphKeyVisitor
		{
		public:
			virtual void Visit(const char*, float) = 0;
		};

		class StringVisitor
		{
		public:
			virtual void Visit(const char*) = 0;
		};

		class ActorVisitor
		{
		public:
			virtual void Visit(RE::TESObjectREFR*) = 0;
		};

		class MorphValueVisitor
		{
		public:
			virtual void Visit(RE::TESObjectREFR*, const char*, const char*, float) = 0;
		};

		class MorphVisitor
		{
		public:
			virtual void Visit(RE::TESObjectREFR*, const char*) = 0;
		};

		virtual void  SetMorph(RE::TESObjectREFR* a_actor, const char* a_name, const char* a_key, float a_relative) = 0;
		virtual float GetMorph(RE::TESObjectREFR* a_actor, const char* a_name, const char* a_key) = 0;
		virtual void  ClearMorph(RE::TESObjectREFR* a_actor, const char* a_name, const char* a_key) = 0;

		virtual float GetBodyMorphs(RE::TESObjectREFR* a_actor, const char* a_name) = 0;
		virtual void  ClearBodyMorphNames(RE::TESObjectREFR* a_actor, const char* a_name) = 0;

		virtual void VisitMorphs(RE::TESObjectREFR* a_actor, MorphVisitor& a_visitor) = 0;
		virtual void VisitKeys(RE::TESObjectREFR* a_actor, const char* a_name, MorphKeyVisitor& a_visitor) = 0;
		virtual void VisitMorphValues(RE::TESObjectREFR* a_actor, MorphValueVisitor& a_visitor) = 0;

		virtual void ClearMorphs(RE::TESObjectREFR* a_actor) = 0;

		virtual void ApplyVertexDiff(RE::TESObjectREFR* a_refr, RE::NiAVObject* a_rootNode, bool a_erase = false) = 0;

		virtual void ApplyBodyMorphs(RE::TESObjectREFR* a_refr, bool a_deferUpdate = true) = 0;
		virtual void UpdateModelWeight(RE::TESObjectREFR* a_refr, bool a_immediate = false) = 0;

		virtual void     SetCacheLimit(size_t a_limit) = 0;
		virtual bool     HasMorphs(RE::TESObjectREFR* a_actor) = 0;
		virtual uint32_t EvaluateBodyMorphs(RE::TESObjectREFR* a_actor) = 0;

		virtual bool   HasBodyMorph(RE::TESObjectREFR* a_actor, const char* a_name, const char* a_key) = 0;
		virtual bool   HasBodyMorphName(RE::TESObjectREFR* a_actor, const char* a_name) = 0;
		virtual bool   HasBodyMorphKey(RE::TESObjectREFR* a_actor, const char* a_key) = 0;
		virtual void   ClearBodyMorphKeys(RE::TESObjectREFR* a_actor, const char* a_key) = 0;
		virtual void   VisitStrings(StringVisitor& a_visitor) = 0;
		virtual void   VisitActors(ActorVisitor& a_visitor) = 0;
		virtual size_t ClearMorphCache() = 0;
	};

	class INiTransformInterface : public IPluginInterface
	{
	public:
		struct Position
		{
			float x, y, z;
		};

		struct Rotation
		{
			float heading, attitude, bank;
		};

		// Visits all overrides within a set
		class NodeVisitor
		{
		public:
			virtual bool VisitPosition(const char* a_node, const char* a_key, Position& a_position) = 0;
			virtual bool VisitRotation(const char* a_node, const char* a_key, Rotation& a_rotation) = 0;
			virtual bool VisitScale(const char* a_node, const char* a_key, float a_scale) = 0;
			virtual bool VisitScaleMode(const char* a_node, const char* a_key, uint32_t a_scaleMode) = 0;
		};

		virtual bool HasNodeTransformPosition(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual bool HasNodeTransformRotation(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual bool HasNodeTransformScale(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual bool HasNodeTransformScaleMode(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;

		virtual void AddNodeTransformPosition(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name, Position& a_position) = 0;  // X,Y,Z
		virtual void AddNodeTransformRotation(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name, Rotation& a_rotation) = 0;  // Euler angles
		virtual void AddNodeTransformScale(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name, float a_scale) = 0;
		virtual void AddNodeTransformScaleMode(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name, uint32_t a_scaleMode) = 0;

		virtual Position GetNodeTransformPosition(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual Rotation GetNodeTransformRotation(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual float    GetNodeTransformScale(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual uint32_t GetNodeTransformScaleMode(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;

		virtual bool RemoveNodeTransformPosition(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual bool RemoveNodeTransformRotation(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual bool RemoveNodeTransformScale(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual bool RemoveNodeTransformScaleMode(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;

		virtual bool RemoveNodeTransform(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name) = 0;
		virtual void RemoveAllReferenceTransforms(RE::TESObjectREFR* a_refr) = 0;

		virtual bool GetOverrideNodeTransform(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node, const char* a_name, uint16_t a_key, RE::NiTransform* a_result) = 0;

		virtual void UpdateNodeAllTransforms(RE::TESObjectREFR* a_refr) = 0;

		virtual void VisitNodes(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, NodeVisitor& a_visitor) = 0;
		virtual void UpdateNodeTransforms(RE::TESObjectREFR* a_refr, bool a_firstPerson, bool a_female, const char* a_node) = 0;
	};

	class IOverlayInterface : public IPluginInterface, public IAddonAttachmentInterface
	{
	public:
		virtual uint32_t GetVersion() = 0;

		virtual void Save(SKSE::SerializationInterface* intfc, uint32_t kVersion) = 0;
		virtual bool Load(SKSE::SerializationInterface* intfc, uint32_t kVersion) = 0;
		virtual void Revert() = 0;

		virtual bool HasOverlays(RE::TESObjectREFR* reference) = 0;
		virtual void AddOverlays(RE::TESObjectREFR* reference) = 0;
		virtual void RemoveOverlays(RE::TESObjectREFR* reference) = 0;
		virtual void InstallOverlay(const char* nodeName, const char* path, RE::TESObjectREFR* refr, RE::BSGeometry* source, RE::NiNode* destination, RE::BSTextureSet* textureSet = NULL) = 0;
	};

	class IAttachmentInterface : public IPluginInterface
	{
	public:
		virtual bool AttachMesh(RE::TESObjectREFR* a_refr, const char* a_nifPath, const char* a_name, bool a_firstPerson, bool a_replace, const char** a_filter, uint32_t a_filterSize, RE::NiAVObject*& a_out, char* a_err, size_t a_errBufLen) = 0;
		virtual bool DetachMesh(RE::TESObjectREFR* a_refr, const char* a_name, bool a_firstPerson) = 0;
	};

	class IActorUpdateManager : public IPluginInterface
	{
	public:
		virtual void AddBodyUpdate(uint32_t a_formId) = 0;
		virtual void AddTransformUpdate(uint32_t a_formId) = 0;
		virtual void AddOverlayUpdate(uint32_t a_formId) = 0;
		virtual void AddNodeOverrideUpdate(uint32_t a_formId) = 0;
		virtual void AddWeaponOverrideUpdate(uint32_t a_formId) = 0;
		virtual void AddAddonOverrideUpdate(uint32_t a_formId) = 0;
		virtual void AddSkinOverrideUpdate(uint32_t a_formId) = 0;
		virtual void Flush() = 0;
	};

	inline IInterfaceMap* GetInterfaceMap()
	{
		InterfaceExchangeMessage msg;
		auto                     sender = InterfaceExchangeMessage::kExchangeInterface;
		auto                     intfc = SKSE::GetMessagingInterface();
		intfc->Dispatch(sender, (void*)&msg, sizeof(InterfaceExchangeMessage*), "SKEE");
		return msg.interfaceMap ? msg.interfaceMap : nullptr;
	}

	inline IBodyMorphInterface* GetBodyMorphInterface(IInterfaceMap* a_map)
	{
		auto intfc = a_map->QueryInterface("BodyMorph");
		return static_cast<IBodyMorphInterface*>(intfc);
	}

	inline INiTransformInterface* GetNiTransformInterface(IInterfaceMap* a_map)
	{
		auto intfc = a_map->QueryInterface("NiTransform");
		return static_cast<INiTransformInterface*>(intfc);
	}

	inline IOverlayInterface* GetOverlayInterface(IInterfaceMap* a_map)
	{
		auto intfc = a_map->QueryInterface("Overlay");
		return static_cast<IOverlayInterface*>(intfc);
	}

	inline IAttachmentInterface* GetAttachmentInterface(IInterfaceMap* a_map)
	{
		auto intfc = a_map->QueryInterface("Attachment");
		return static_cast<IAttachmentInterface*>(intfc);
	}

	inline IActorUpdateManager* GetActorUpdateManager(IInterfaceMap* a_map)
	{
		auto intfc = a_map->QueryInterface("ActorUpdateManager");
		return static_cast<IActorUpdateManager*>(intfc);
	}
}
