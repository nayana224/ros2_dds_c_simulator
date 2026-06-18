/**
 * @file simulator_internal.h
 * @brief Internal declarations shared across simulator implementation files.
 *
 * This header is not part of the public API. It contains helpers that are
 * used by multiple simulator source files after the implementation was split
 * by responsibility.
 */

#ifndef SIMULATOR_INTERNAL_H
#define SIMULATOR_INTERNAL_H

#include "simulator.h"

/**
 * @brief Validates a node or topic name.
 *
 * @param name Name string to validate.
 * @return 1 if the name is valid, or 0 otherwise.
 */
int is_valid_name(const char *name);

/**
 * @brief Finds a publisher entry by node name within a topic.
 *
 * @param topic Topic to search.
 * @param node_name Publisher node name to search for.
 * @return A pointer to the matching publisher entry, or NULL if not found.
 */
Publisher *find_publisher_in_topic(const Topic *topic, const char *node_name);

/**
 * @brief Finds a subscriber entry by node name within a topic.
 *
 * @param topic Topic to search.
 * @param node_name Subscriber node name to search for.
 * @return A pointer to the matching subscriber entry, or NULL if not found.
 */
Subscriber *find_subscriber_in_topic(const Topic *topic, const char *node_name);

#endif
