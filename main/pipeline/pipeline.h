#ifndef RUBY_TYPER_PIPELINE_H
#define RUBY_TYPER_PIPELINE_H
#include "ProgressIndicator.h"
#include "ast/ast.h"
#include "common/common.h"
#include "common/concurrency/WorkerPool.h"
#include "common/kvstore/KeyValueStore.h"
#include "main/options/options.h"

namespace sorbet::core::lsp {
class PreemptionTaskManager;
}

namespace sorbet::realmain::pipeline {

struct IndexResult {
    std::unique_ptr<core::GlobalState> gs;
    std::vector<ast::ParsedFile> trees;
    std::vector<ast::CompressedParsedFile> compressedTrees;
};

ast::ParsedFile indexOne(const options::Options &opts, core::GlobalState &lgs, core::FileRef file);

std::vector<core::FileRef> reserveFiles(std::unique_ptr<core::GlobalState> &gs, const std::vector<std::string> &files);

std::vector<ast::ParsedFile> index(std::unique_ptr<core::GlobalState> &gs, std::vector<core::FileRef> files,
                                   const options::Options &opts, WorkerPool &workers,
                                   const std::unique_ptr<const OwnedKeyValueStore> &kvstore);

std::vector<ast::ParsedFile> package(core::GlobalState &gs, std::vector<ast::ParsedFile> what,
                                     const options::Options &opts, WorkerPool &workers);

ast::ParsedFilesOrCancelled resolve(std::unique_ptr<core::GlobalState> &gs, std::vector<ast::ParsedFile> what,
                                    const options::Options &opts, WorkerPool &workers);

std::vector<ast::ParsedFile> incrementalResolve(core::GlobalState &gs, std::vector<ast::ParsedFile> what,
                                                const options::Options &opts);

ast::ParsedFilesOrCancelled name(core::GlobalState &gs, std::vector<ast::ParsedFile> what, const options::Options &opts,
                                 WorkerPool &workers);

// Note: `cancelable` and `preemption task manager` are only applicable to LSP.
ast::ParsedFilesOrCancelled
typecheck(std::unique_ptr<core::GlobalState> &gs, std::vector<ast::ParsedFile> what, const options::Options &opts,
          WorkerPool &workers, bool cancelable = false,
          std::optional<std::shared_ptr<core::lsp::PreemptionTaskManager>> preemptionManager = std::nullopt,
          bool presorted = false);

ast::ParsedFile typecheckOne(core::Context ctx, ast::ParsedFile resolved, const options::Options &opts);

core::StrictLevel decideStrictLevel(const core::GlobalState &gs, const core::FileRef file,
                                    const options::Options &opts);

// Caches any uncached trees and files. Returns true if it modifies kvstore.
bool cacheTreesAndFiles(const core::GlobalState &gs, WorkerPool &workers, std::vector<ast::ParsedFile> &parsedFiles,
                        const std::unique_ptr<OwnedKeyValueStore> &kvstore);

// Exported for tests only.
std::string fileKey(const core::File &file);
std::string treeKey(const core::File &file);

} // namespace sorbet::realmain::pipeline
#endif // RUBY_TYPER_PIPELINE_H
