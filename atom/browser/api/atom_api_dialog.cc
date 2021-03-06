// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>
#include <utility>
#include <vector>

#include "atom/browser/api/atom_api_browser_window.h"
#include "atom/browser/native_window.h"
#include "atom/browser/ui/certificate_trust.h"
#include "atom/browser/ui/file_dialog.h"
#include "atom/browser/ui/message_box.h"
#include "atom/common/native_mate_converters/callback.h"
#include "atom/common/native_mate_converters/file_dialog_converter.h"
#include "atom/common/native_mate_converters/file_path_converter.h"
#include "atom/common/native_mate_converters/image_converter.h"
#include "atom/common/native_mate_converters/net_converter.h"
#include "native_mate/dictionary.h"

#include "atom/common/node_includes.h"

namespace {

void ShowMessageBox(int type,
                    const std::vector<std::string>& buttons,
                    int default_id,
                    int cancel_id,
                    int options,
                    const std::string& title,
                    const std::string& message,
                    const std::string& detail,
                    const std::string& checkbox_label,
                    bool checkbox_checked,
                    const gfx::ImageSkia& icon,
                    atom::NativeWindow* window,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  atom::MessageBoxCallback callback;
  if (mate::Converter<atom::MessageBoxCallback>::FromV8(args->isolate(), peek,
                                                        &callback)) {
    atom::ShowMessageBox(window, static_cast<atom::MessageBoxType>(type),
                         buttons, default_id, cancel_id, options, title,
                         message, detail, checkbox_label, checkbox_checked,
                         icon, callback);
  } else {
    int chosen = atom::ShowMessageBox(
        window, static_cast<atom::MessageBoxType>(type), buttons, default_id,
        cancel_id, options, title, message, detail, icon);
    args->Return(chosen);
  }
}

void ShowOpenDialog(const file_dialog::DialogSettings& settings,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  file_dialog::OpenDialogCallback callback;
  if (mate::Converter<file_dialog::OpenDialogCallback>::FromV8(
          args->isolate(), peek, &callback)) {
    file_dialog::ShowOpenDialog(settings, callback);
  } else {
    std::vector<base::FilePath> paths;
    if (file_dialog::ShowOpenDialog(settings, &paths))
      args->Return(paths);
  }
}

void ShowSaveDialog(const file_dialog::DialogSettings& settings,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  file_dialog::SaveDialogCallback callback;
  if (mate::Converter<file_dialog::SaveDialogCallback>::FromV8(
          args->isolate(), peek, &callback)) {
    file_dialog::ShowSaveDialog(settings, callback);
  } else {
    base::FilePath path;
    if (file_dialog::ShowSaveDialog(settings, &path))
      args->Return(path);
  }
}

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  mate::Dictionary dict(context->GetIsolate(), exports);
  dict.SetMethod("showMessageBox", &ShowMessageBox);
  dict.SetMethod("showErrorBox", &atom::ShowErrorBox);
  dict.SetMethod("showOpenDialog", &ShowOpenDialog);
  dict.SetMethod("showSaveDialog", &ShowSaveDialog);
#if defined(OS_MACOSX) || defined(OS_WIN)
  dict.SetMethod("showCertificateTrustDialog",
                 &certificate_trust::ShowCertificateTrust);
#endif
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(atom_browser_dialog, Initialize)
