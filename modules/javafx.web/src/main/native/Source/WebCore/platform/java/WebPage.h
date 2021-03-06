/*
 * Copyright (c) 2012, 2017, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#pragma once

#if USE(ACCELERATED_COMPOSITING)
#include <UnicodeJava.h>
#include "GraphicsLayerClient.h"
#endif
#include "IntRect.h"
#include "PrintContext.h"
#include "RQRef.h"
#include "ScrollTypes.h"

#include <jni.h> // todo tav remove when building w/ pch
#include <wtf/java/JavaRef.h>


namespace WebCore {

class Frame;
class GraphicsContext;
class GraphicsLayer;
class IntRect;
class IntSize;
class Node;
class Page;
class PlatformKeyboardEvent;
class TextureMapper;

class WebPage
#if USE(ACCELERATED_COMPOSITING)
    : GraphicsLayerClient
#endif
{
public:
    WebPage(std::unique_ptr<Page> page);
    ~WebPage();

    inline Page* page()
    {
        return m_page.get();
    }

    static inline WebPage* webPageFromJLong(jlong p)
    {
        return static_cast<WebPage*>(jlong_to_ptr(p));
    }

    static WebPage* webPageFromJObject(const JLObject& obj);

    static inline Page* pageFromJLong(jlong p)
    {
        WebPage* webPage = webPageFromJLong(p);
        return webPage ? webPage->page() : NULL;
    }

    static inline Page* pageFromJObject(const JLObject& obj)
    {
        WebPage* webPage = webPageFromJObject(obj);
        return webPage ? webPage->page() : NULL;
    }

    static JLObject jobjectFromPage(Page* page);

    void setSize(const IntSize&);
    void prePaint();
    void paint(jobject, jint, jint, jint, jint);
    void postPaint(jobject, jint, jint, jint, jint);
    bool processKeyEvent(const PlatformKeyboardEvent& event);

    void scroll(const IntSize& scrollDelta, const IntRect& rectToScroll,
                const IntRect& clipRect);
    void repaint(const IntRect&);
    int beginPrinting(float width, float height);
    void print(GraphicsContext& gc, int pageIndex, float pageWidth);
    void endPrinting();
#if USE(ACCELERATED_COMPOSITING)
    void setRootChildLayer(GraphicsLayer*);
    void setNeedsOneShotDrawingSynchronization();
    void scheduleCompositingLayerSync();
#endif
    void debugStarted();
    void debugEnded();
    void enableWatchdog();
    void disableWatchdog();

private:
    void requestJavaRepaint(const IntRect&);
#if USE(ACCELERATED_COMPOSITING)
    void markForSync();
    void syncLayers();
    IntRect pageRect();
    void renderCompositedLayers(GraphicsContext&, const IntRect&);

    // GraphicsLayerClient
    virtual void notifyAnimationStarted(const GraphicsLayer*, double);
    virtual void notifyFlushRequired(const GraphicsLayer*);
    virtual void paintContents(const GraphicsLayer*,
                               GraphicsContext&,
                               GraphicsLayerPaintingPhase,
                               const FloatRect&);
    virtual bool showDebugBorders(const GraphicsLayer*) const;
    virtual bool showRepaintCounter(const GraphicsLayer*) const;
#endif

    bool keyEvent(const PlatformKeyboardEvent& event);
    bool charEvent(const PlatformKeyboardEvent& event);
    bool keyEventDefault(const PlatformKeyboardEvent& event);
    bool scrollViewWithKeyboard(int keyCode, const PlatformKeyboardEvent& event);
    static bool mapKeyCodeForScroll(int keyCode,
                                    ScrollDirection* scrollDirection,
                                    ScrollGranularity* scrollGranularity);
    bool propagateScroll(ScrollDirection scrollDirection,
                         ScrollGranularity scrollGranularity);
    Frame* focusedWebCoreFrame();
    Node* focusedWebCoreNode();

    std::unique_ptr<Page> m_page;
    std::unique_ptr<PrintContext> m_printContext;
    RefPtr<RQRef> m_jTheme;

#if USE(ACCELERATED_COMPOSITING)
    std::unique_ptr<GraphicsLayer> m_rootLayer;
    std::unique_ptr<TextureMapper> m_textureMapper;
    bool m_syncLayers;
#endif

    // Webkit expects keyPress events to be suppressed if the associated keyDown
    // event was handled. Safari implements this behavior by peeking out the
    // associated WM_CHAR event if the keydown was handled. We emulate
    // this behavior by setting this flag if the keyDown was handled.
    bool m_suppressNextKeypressEvent;

    bool m_isDebugging;
    static int globalDebugSessionCounter;
};

} // namespace WebCore

