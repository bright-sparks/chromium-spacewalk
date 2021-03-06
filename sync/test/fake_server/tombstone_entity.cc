// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sync/test/fake_server/tombstone_entity.h"

#include <string>

#include "base/basictypes.h"
#include "sync/internal_api/public/base/model_type.h"
#include "sync/protocol/sync.pb.h"
#include "sync/test/fake_server/fake_server_entity.h"

using std::string;

using syncer::ModelType;

namespace fake_server {

TombstoneEntity::~TombstoneEntity() { }

// static
FakeServerEntity* TombstoneEntity::Create(const string& id) {
  return new TombstoneEntity(id, GetModelTypeFromId(id));
}

TombstoneEntity::TombstoneEntity(const string& id,
                                 const ModelType& model_type)
    : FakeServerEntity(id, model_type, 0, string()) { }

string TombstoneEntity::GetParentId() const {
  return string();
}

sync_pb::SyncEntity* TombstoneEntity::SerializeAsProto() {
  sync_pb::SyncEntity* sync_entity = new sync_pb::SyncEntity();
  FakeServerEntity::SerializeBaseProtoFields(sync_entity);

  sync_pb::EntitySpecifics* specifics = sync_entity->mutable_specifics();
  AddDefaultFieldValue(FakeServerEntity::GetModelType(), specifics);

  return sync_entity;
}

bool TombstoneEntity::IsDeleted() const {
  return true;
}

bool TombstoneEntity::IsFolder() const {
  return false;
}

}  // namespace fake_server
