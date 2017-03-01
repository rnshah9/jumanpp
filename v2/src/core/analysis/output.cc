//
// Created by Arseny Tolmachev on 2017/03/01.
//

#include "output.h"

namespace jumanpp {
namespace core {
namespace analysis {

Status OutputManager::stringField(StringPiece name, StringField *result) const {
  auto fld = holder_->fieldByName(name);
  if (fld == nullptr) {
    return Status::InvalidParameter() << "dictionary field with name " << name
                                      << " was not found";
  }
  if (fld->columnType != spec::ColumnType::String) {
    return Status::InvalidParameter() << "field " << name
                                      << " was not string typed";
  }
  result->initialize(fld->index, xtra_, fld->strings);
  return Status::Ok();
}

bool OutputManager::fillEntry(EntryPtr ptr,
                              util::MutableArraySlice<i32> entries) const {
  if (ptr.isSpecial()) {
    auto node = xtra_->node(ptr);
    if (node == nullptr) {
      return false;
    }
    if (node->header.type == ExtraNodeType::Unknown) {
      auto data = xtra_->nodeContent(node);
      std::copy(data.begin(), data.end(), entries.begin());
      return true;
    }
  } else {
    auto entry = entries_.entryAtPtr(ptr.dicPtr());
    entry.fill(entries, entries.size());
    return true;
  }
  return false;
}

NodeWalker OutputManager::nodeWalker() const {
  auto size = (size_t)entries_.entrySize();
  util::MutableArraySlice<i32> slice{alloc_->allocateArray<i32>(size), size};
  return NodeWalker{slice};
}

bool OutputManager::locate(LatticeNodePtr ptr, NodeWalker* result) const {
  auto bnd = lattice_->boundary(ptr.boundary);
  auto eptr = bnd->entry(ptr.position);
  return locate(eptr, result);
}

bool OutputManager::locate(EntryPtr ptr, NodeWalker *result) const {
  result->mgr_ = this;
  result->status_ = NodeLookupStatus::Failure;
  if (ptr.isSpecial()) {
    auto node = xtra_->node(ptr);
    if (node == nullptr) {

      return false;
    }
    if (node->header.type == ExtraNodeType::Alias) {
      result->status_ = NodeLookupStatus::Multiple;
      auto& hdr = node->header.alias;
      result->remaining_ = (i32) hdr.dictionaryNodes.size();
      result->nodes_ = hdr.dictionaryNodes;
      return true;
    }

    if (node->header.type == ExtraNodeType::Unknown) {
      result->status_ = NodeLookupStatus::Single;
      result->remaining_ = 1;
      auto data = xtra_->nodeContent(node);
      std::copy(data.begin(), data.end(), result->values_.begin());
      return true;
    }

  } else {
    auto entry = entries_.entryAtPtr(ptr.dicPtr());
    entry.fill(result->values_, result->values_.size());
    result->remaining_ = 1;
    result->status_ = NodeLookupStatus::Single;
    return true;
  }
  return false;
}

bool NodeWalker::handleMultiple() {
  EntryPtr eptr{nodes_.at(nodes_.size() - remaining_)};
  return mgr_->fillEntry(eptr, values_);
}

StringPiece StringField::operator[](const NodeWalker &node) const {
  i32 value = 0;
  if (node.valueOf(index_, &value)) {
    if (value < 0) {
      return xtra_->string(value);
    }
    StringPiece result;
    if (reader_.readAt(value, &result)) {
      return result;
    }
    JPP_DCHECK_NOT("should fail in debug");
    return "----READ_ERROR!!!----";
  }
  JPP_DCHECK_NOT("should fail in debug");
  return "-----STRING_FIELD_ERROR!!!----";
}

void StringField::initialize(i32 index, const ExtraNodesContext *xtra, dic::impl::StringStorageReader reader) {
  index_ = index;
  xtra_ = xtra;
  new (&reader_) dic::impl::StringStorageReader{reader};
}

}  // analysis
}  // core
}  // jumanpp