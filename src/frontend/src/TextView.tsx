import CodeMirror, { highlightActiveLine } from "@uiw/react-codemirror"
import { shadcnTheme } from "./editor-theme";


export function TextView() {
  return (
    <CodeMirror theme={shadcnTheme} basicSetup={{
      lineNumbers: false,
      highlightActiveLine: false,
      foldGutter: false,
      closeBrackets: true,
    }}/>
  );
};
