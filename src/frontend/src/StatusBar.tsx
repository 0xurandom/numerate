import { useState } from "react";

interface StatusBarProps {
  pageName: string;
  currentPage: number;
  pageCount: number;
  onPageNameChange: (name: string) => void;
}

export function StatusBar({
  pageName,
  currentPage,
  pageCount,
  onPageNameChange,
}: StatusBarProps) {
  const [showShortcuts, setShowShortcuts] = useState(false);

  return (
    <>
      <div className="flex h-8 items-center justify-between border-t px-3 text-sm">
        <input
          value={pageName}
          onChange={(event) => onPageNameChange(event.target.value)}
          className="min-w-0 bg-transparent outline-none"
          placeholder="Page name"
        />

        <div className="flex items-center gap-4 whitespace-nowrap">
          <span>
            Page {currentPage + 1} of {pageCount}
          </span>

          <button
            onClick={() => setShowShortcuts((show) => !show)}
            className="rounded px-2 py-1 hover: bg-muted"
            title="Keyboard shortcuts"
          >
            ?
          </button>
        </div>
      </div>


      {showShortcuts && (
        <div className="rounded border absolute bg-background p-3 text-sm shadow-lg bottom-10 right-3">
          <div className="font-semibold mb-2">
            Keyboard Shortcuts
          </div>
          <div className="space-y-1">
            <div>Ctrl  +  ←  : Previous Page</div>
            <div>Ctrl  +  →  : Next Page</div>
            <div>Ctrl  +  N  : New Page</div>
            <div>Ctrl  +  +  : Zoom in</div>
            <div>Ctrl  +  -  : Zoom out</div>
            <div>Ctrl  +  0  : Reset zoom</div>
          </div>
        </div>
      )}

    </>
  )
}
