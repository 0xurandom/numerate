import CodeMirror, { highlightActiveLine } from "@uiw/react-codemirror"

export function TextView (){
  return (
    <CodeMirror basicSetup={{
      lineNumbers: false,
      highlightActiveLine: false,
    }}/>
  );
};
