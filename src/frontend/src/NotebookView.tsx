import { ScrollArea } from "./components/ui/scroll-area";
import { FileText, Clock } from "lucide-react";
import { Card, CardFooter, CardHeader, CardTitle } from "./components/ui/card";
import { initStorage, Notebook, getNotebooks } from "./notebooks";
import { useState, useEffect } from "react";

function getDate(timestamp: number): string  {
  return new Intl.DateTimeFormat('en-GB', {
    month: 'short',
    day: 'numeric',
    hour: 'numeric',
    minute: 'numeric'
  }).format(new Date(timestamp))
}

export function NotebookView() {
  const [notebooks, setNotebooks] = useState<Notebook[]>([]);

  useEffect(() => {
    initStorage();
    getNotebooks().then(setNotebooks);
  }, []);

  return (
    <div>
      <ScrollArea>
        {notebooks.length == 0 ? (
          <div>
            <FileText className="text-muted-foreground/50" />
              <h3>No notebooks found</h3>
          </div>
        ) : (
          <div className="grid grid-cols-1 gap-6 pb-8">
            {notebooks.map((notebook) => (
              <Card key={notebook.id} onClick={() => null}>
                <CardHeader className="pb-8">
                  <div>
                    <CardTitle className="text-xl">
                      {notebook.title}
                    </CardTitle>
                  </div>
                  <div>
                    <Clock />
                    {getDate(notebook.lastModified)}
                  </div>
                </CardHeader>
                <CardFooter className="pt-0 mt-auto">
                  <div className="absolute inset-0 bg-muted/30 rounded-md overflow-hidden text-xs text-muted font-mono">
                    {(notebook.content == '') ? 'Empty notebook...' : notebook.content}
                  </div>
                </CardFooter>
              </Card>
            ))}
          </div>
        )}
      </ScrollArea>
    </div>
  );
}
