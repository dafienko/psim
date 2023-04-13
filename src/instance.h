#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "event.h"

using json = nlohmann::json; 

class Instance {
	public:
		enum InstanceClass {
			IC_UI,
			IC_UIObject,
			IC_UIFrame,
			IC_UIText,
		};

		/**
		 * @brief Generate an instance from a json file
		 * 
		 * @param path the path to the json file
		 * @return Instance* 
		 */
		static Instance* fromJSON(const std::string& path);
	private:
		std::vector<Instance*> children;
		Instance* parent = nullptr;

		Event<Instance&, Instance&> childAdded;
		Event<Instance&, Instance&> childRemoved;

		void addChild(Instance& child);
		void removeChild(Instance& child);
		
	public:
		//! The type of instance that this is
		const InstanceClass type;
		//! The name of the instance
		std::string name = "";

		/**
		 * @brief Construct a new Instance object
		 * 
		 * @param type the type of instance to create
		 * @param name the name to initialize the instance with
		 */
		Instance(InstanceClass type, const std::string& name);

		/**
		 * @brief Set this instance's parent
		 * 
		 * @param parent the new parent instance (or null)
		 */
		void setParent(Instance* parent);

		/**
		 * @brief Get this instance's current parent
		 * 
		 * @return Instance*
		 */
		Instance* getParent();

		/**
		 * @brief Get all the children of this instance
		 * 
		 * @return std::vector<Instance*> 
		 */
		std::vector<Instance*> getChildren() const;

		/**
		 * @brief Return the child with the specified name or null if none exists
		 * 
		 * @param name the name of the child to look for
		 * @return Instance* 
		 */
		Instance* findChild(const std::string& name) const;
		
		/**
		 * @brief Return the child with the specified name or error if none exists
		 * 
		 * @param name the name of the child to look for
		 * @return Instance* 
		 */
		Instance* getChild(const std::string& name) const;

		virtual ~Instance();
};